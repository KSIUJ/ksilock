#![feature(ip_addr)]

use std::net::UdpSocket;
use std::net::SocketAddr;
use std::net::IpAddr;
use std::str::FromStr;
use std::fmt;

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

fn get_message_from_socket(sock: &UdpSocket) -> Message {
  let mut buff = [0; 128];
  let (amt, src) = sock.recv_from(&mut buff).unwrap();
  let msg_type = buff[0];
  match msg_type {
    1 => Message::Auth {
          src: src,
          amt: amt,
          card_id: buff[1..5].iter()
                             .map(|b| format!("{:02X}", b))
                             .collect(),
        },
    _ => panic!("Unknown packet type. Panic for now..."),
  }
}

fn main() {
  let lock_ip: IpAddr = FromStr::from_str("192.168.88.37").unwrap();
  let lock_addr = SocketAddr::new(lock_ip, 7777);

  let srv_ip: IpAddr = FromStr::from_str("192.168.88.4").unwrap();
  let srv_addr = SocketAddr::new(srv_ip, 7777);

  let srv_sock = UdpSocket::bind(srv_addr).unwrap();

  loop {
    let msg = get_message_from_socket(&srv_sock);
    println!("{:?}", msg);
  }
}
