#![feature(ip_addr)]
#![feature(convert)]

extern crate config;
extern crate postgres;

use std::net::UdpSocket;
use std::net::SocketAddr;
use std::net::IpAddr;
use std::str::FromStr;
use std::fmt;
use std::io;

enum Message {
  Auth {
    src: SocketAddr,
    amt: usize,
    card_id: String,
  },
}

impl fmt::Debug for Message {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    match *self {
      Message::Auth { src, amt, ref card_id } =>
                      f.debug_struct("Message::Auth")
                        .field("From", &src)
                        .field("Bytes received", &amt)
                        .field("Card id", &card_id)
                        .finish(),
    }
  }
}

fn get_message_from_socket(sock: &UdpSocket) -> io::Result<Message> {
  let mut buff = [0; 128];
  let (amt, src) = try!(sock.recv_from(&mut buff));
  let msg_type = buff[0];
  match msg_type {
    1 => Ok(Message::Auth {
            src: src,
            amt: amt,
            card_id: buff[1..5].iter()
                               .map(|b| format!("{:02X}", b))
                               .collect(),
         }),
    _ => Err(io::Error::new(io::ErrorKind::InvalidData,
                            "Packet type not known.")),
  }
}

fn main() {
  use std::path::Path;
  use config::reader;
  use postgres::{Connection, SslMode};
  
  let config_path = Path::new("gatekeeper.cfg");
  let config = reader::from_file(config_path)
                        .unwrap_or_else(
                          |e| panic!("Config file not found or malformed.\n\
                                     Original error: {:?}", e));
  
  let port = config.lookup_integer32("port")
                   .unwrap_or_else(|| panic!("port var not found."))
                   as u16;
  let lock_ip_str = config.lookup_str("lock_ip")
                          .unwrap_or_else(|| panic!("lock_ip var not found."));
  let lock_ip: IpAddr = FromStr::from_str(lock_ip_str)
                                  .unwrap_or_else(
                                    |e| panic!("Error parsing lock ip.\n\
                                               Original error {:?}", e));
  let lock_addr = SocketAddr::new(lock_ip, port);
  let srv_ip_str = config.lookup_str("srv_ip")
                          .unwrap_or_else(|| panic!("srv_ip var not found."));
  let srv_ip: IpAddr = FromStr::from_str(srv_ip_str)
                                  .unwrap_or_else(
                                    |e| panic!("Error parsing srv ip.\n\
                                               Original error {:?}", e));
  let srv_addr = SocketAddr::new(srv_ip, port);
  let srv_sock = UdpSocket::bind(srv_addr)
                              .unwrap_or_else(
                                |e| panic!("Failed to bind to socket.\n\
                                           Original error: {:?}", e));

  let dbhost = config.lookup_str("dbhost").unwrap();
  let dbname = config.lookup_str("dbname").unwrap();
  let dbuser = config.lookup_str("dbuser").unwrap();
  let dbpass = config.lookup_str("dbpass").unwrap();
  let conn = Connection::connect(format!("postgres://{}:{}@{}/{}",
                                         dbuser,
                                         dbpass,
                                         dbhost,
                                         dbname).as_str(),
                                 &SslMode::None)
                          .unwrap();

  let auth_statement = conn.prepare("SELECT name, surname, card_id FROM USERS \
                                     WHERE card_id = '$1'").unwrap();

  loop {
    match get_message_from_socket(&srv_sock) {
      Ok(msg) => {  
        println!("{:?}", msg);
        match msg {
          Message::Auth { card_id, .. } => {
            let rows = auth_statement.query(&[&card_id]).unwrap();
            if rows.len() == 1 {
              let row = rows.get(0);
              let name: String = row.get(0);
              let surname: String = row.get(1);
              println!("Access granted for user {} {}.", name, surname);
            } else {
              println!("Unknown card with id: {}.", card_id);
            }
          },
        }
      },
      Err(e) => println!("{:?}", e),
    }
  }
}
