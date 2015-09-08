#![feature(ip_addr)]

extern crate config;

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

  loop {
    match get_message_from_socket(&srv_sock) {
      Ok(msg) => println!("{:?}", msg),
      Err(e) => println!("{:?}", e),
    }
  }
}
