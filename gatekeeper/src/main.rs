#![feature(ip_addr)]
#![feature(convert)]

extern crate config;
extern crate postgres;
extern crate crypto;

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
  use crypto::{aes, blockmodes, buffer};
  use crypto::symmetriccipher::Decryptor;
  
  let key = [
    0x52, 0x19, 0x08, 0xA8, 0x2F, 0x52, 0x78, 0x6E,
    0x80, 0x6D, 0x33, 0x34, 0xC4, 0x81, 0x86, 0xA4,
    0x0B, 0xAA, 0x40, 0x0D, 0x1A, 0x3F, 0xF4, 0x66,
    0x83, 0xD7, 0x34, 0x17, 0x9A, 0x67, 0x41, 0xF4,
  ];
  let mut decryptor = aes::ecb_decryptor(aes::KeySize::KeySize256,
                                     &key,
                                     blockmodes::NoPadding);

  let mut encrypted = [0; 128];
  let mut decrypted = [0; 128];
  let (amt, src) = try!(sock.recv_from(&mut encrypted));
  {
    let mut decrypted_buff = buffer::RefWriteBuffer::new(&mut decrypted);
    let mut encrypted_buff = buffer::RefReadBuffer::new(&encrypted);
    decryptor.decrypt(&mut encrypted_buff, &mut decrypted_buff, true);
  }
  let msg_type = decrypted[0];
  match msg_type {
    1 => Ok(Message::Auth {
            src: src,
            amt: amt,
            card_id: decrypted[1..5].iter()
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
                                     WHERE card_id = $1").unwrap();

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
