-- Create syntax for TABLE 'llgcm_app'
CREATE TABLE `llgcm_app` (
  `no` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `app_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `app_id` varchar(50) DEFAULT NULL,
  `auth_key` varchar(100) DEFAULT NULL,
  `add_date` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`no`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

-- Create syntax for TABLE 'llgcm_message'
CREATE TABLE `llgcm_message` (
  `no` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `message` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `reg_date` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`no`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- Create syntax for TABLE 'llgcm_queue'
CREATE TABLE `llgcm_queue` (
  `no` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `send_group_no` int(11) DEFAULT '0',
  `app_no` int(11) DEFAULT NULL,
  `user_no` int(11) DEFAULT NULL,
  `message_no` int(11) DEFAULT '0',
  `status` enum('pending','done') CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT 'pending',
  `add_date` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`no`),
  KEY `idx_pending` (`status`),
  KEY `idx_user_no` (`user_no`),
  KEY `idx_app_no` (`app_no`),
  KEY `idx_send_group_no` (`send_group_no`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

-- Create syntax for TABLE 'llgcm_record'
CREATE TABLE `llgcm_record` (
  `no` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `send_group_no` int(11) unsigned NOT NULL,
  `result_log` text CHARACTER SET utf8 COLLATE utf8_unicode_ci,
  `total_cnt` int(11) DEFAULT '0',
  `success_cnt` int(11) DEFAULT '0',
  `faild_cnt` int(11) DEFAULT '0',
  `add_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Create syntax for TABLE 'llgcm_user'
CREATE TABLE `llgcm_user` (
  `no` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `app_no` int(11) DEFAULT NULL,
  `regstration_id` varchar(255) DEFAULT NULL,
  `status` enum('active','inactive') DEFAULT 'active',
  `add_date` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`no`),
  UNIQUE KEY `idx_regstration_id` (`regstration_id`),
  KEY `idx_status` (`status`),
  KEY `idx_app_no` (`app_no`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
