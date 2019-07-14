
CREATE TABLE `user_info` (
  `id` int(11) DEFAULT NULL,
  `name` varchar(50) DEFAULT NULL,
  `age` int(3) DEFAULT NULL,
  `state_id` int(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 |


 CREATE TABLE `user_action` (
  `id` int(11) DEFAULT NULL,
  `name` varchar(50) DEFAULT NULL,
  `action` int(11) DEFAULT NULL,
  `creat_time` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8 |
