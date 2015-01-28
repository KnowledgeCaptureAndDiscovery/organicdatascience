CREATE DATABASE `wttracking` /*!40100 DEFAULT CHARACTER SET utf8 */;

CREATE TABLE `track` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `action` varchar(200) DEFAULT NULL,
  `actiontype` varchar(200) DEFAULT NULL,
  `user` varchar(200) DEFAULT NULL,
  `time` timestamp NULL DEFAULT NULL,
  `value` text,
  `component` varchar(200) DEFAULT NULL,
  `subcomponent` varchar(200) DEFAULT NULL,
  `taskId` varchar(200) DEFAULT NULL,
  `ip` varchar(200) DEFAULT NULL,
  `winx` int(5) DEFAULT NULL,
  `winy` int(5) DEFAULT NULL,
  `screenx` int(5) DEFAULT NULL,
  `screeny` int(5) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4528 DEFAULT CHARSET=utf8;
