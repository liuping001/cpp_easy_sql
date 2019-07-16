
CREATE TABLE `user_info` (
  `user_id` int(11) AUTO_INCREMENT PRIMARY KEY,
  `name` varchar(50) NOT NULL,
  `passwd` varchar(50) NOT NULL,
  `state_id` int(4) NOT NULL ,
  `creat_time` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
