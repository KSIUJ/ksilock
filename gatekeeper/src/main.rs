#![feature(ip_addr)]

use std::net::UdpSocket;
use std::net::SocketAddr;
use std::net::IpAddr;
use std::str::FromStr;
use std::fmt;

struct Message {
  amt: usize,
  src: SocketAddr,
  data: [u8; 128],
}

impl fmt::Debug for Message {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    f.debug_struct("Message")
      .field("amt", &self.amt)
      .field("src", &self.src)
      .finish()
  }
}

fn get_message_from_socket(sock: &UdpSocket) -> Message {
  let mut buff = [0; 128];
  let (amt, src) = sock.recv_from(&mut buff).unwrap();
  Message {
    amt: amt,
    src: src,
    data: buff,
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
