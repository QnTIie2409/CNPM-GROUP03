-- MySQL dump 10.13  Distrib 8.0.46, for Win64 (x86_64)
--
-- Host: 127.0.0.1    Database: student_attendance
-- ------------------------------------------------------
-- Server version	8.0.46

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `student_attendance`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `student_attendance` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE `student_attendance`;

--
-- Table structure for table `attendance_change_logs`
--

DROP TABLE IF EXISTS `attendance_change_logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `attendance_change_logs` (
  `log_id` int NOT NULL AUTO_INCREMENT,
  `record_id` int NOT NULL,
  `old_status` enum('PRESENT','LATE','ABSENT','EXCUSED') COLLATE utf8mb4_unicode_ci NOT NULL,
  `new_status` enum('PRESENT','LATE','ABSENT','EXCUSED') COLLATE utf8mb4_unicode_ci NOT NULL,
  `reason` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `changed_at` datetime NOT NULL,
  `user_id` int NOT NULL,
  PRIMARY KEY (`log_id`),
  KEY `fk_logs_record` (`record_id`),
  KEY `fk_logs_lecturer` (`user_id`),
  CONSTRAINT `fk_logs_lecturer` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE RESTRICT,
  CONSTRAINT `fk_logs_record` FOREIGN KEY (`record_id`) REFERENCES `attendance_records` (`record_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `attendance_change_logs`
--

LOCK TABLES `attendance_change_logs` WRITE;
/*!40000 ALTER TABLE `attendance_change_logs` DISABLE KEYS */;
INSERT INTO `attendance_change_logs` VALUES (1,10,'ABSENT','PRESENT','Mã điểm danh bị lỗi mạng, đã kiểm tra lại','2026-09-01 20:13:00',3),(2,6,'LATE','PRESENT','Đến trễ do kẹt xe, giảng viên đồng ý tính có mặt','2026-09-02 15:33:00',3),(3,40,'ABSENT','PRESENT','Sinh viên quên nhập mã, giảng viên xác nhận có mặt','2026-09-03 10:26:00',2),(4,43,'LATE','PRESENT','Điều chỉnh theo xác nhận của giảng viên','2026-09-04 05:06:00',2),(5,55,'ABSENT','EXCUSED','Nghỉ có lý do: đi khám bệnh','2026-09-04 11:43:00',3),(6,102,'ABSENT','PRESENT','Mã điểm danh bị lỗi mạng, đã kiểm tra lại','2026-09-17 11:51:00',2),(7,153,'LATE','PRESENT','Đến trễ do kẹt xe, giảng viên đồng ý tính có mặt','2026-09-30 16:01:00',3),(8,156,'LATE','PRESENT','Điều chỉnh theo xác nhận của giảng viên','2026-09-30 18:08:00',3),(9,173,'ABSENT','EXCUSED','Nghỉ có phép của khoa','2026-09-30 19:32:00',2),(10,205,'LATE','PRESENT','Đến trễ do kẹt xe, giảng viên đồng ý tính có mặt','2026-10-01 20:21:00',3),(11,217,'LATE','PRESENT','Đến trễ do kẹt xe, giảng viên đồng ý tính có mặt','2026-10-02 03:57:00',3),(12,222,'ABSENT','EXCUSED','Sinh viên nộp giấy xin phép có xác nhận','2026-10-13 15:04:00',3),(13,220,'ABSENT','PRESENT','Sinh viên quên nhập mã, giảng viên xác nhận có mặt','2026-10-14 02:17:00',3),(14,317,'ABSENT','EXCUSED','Nghỉ có phép của khoa','2026-10-29 19:47:00',2);
/*!40000 ALTER TABLE `attendance_change_logs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `attendance_records`
--

DROP TABLE IF EXISTS `attendance_records`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `attendance_records` (
  `record_id` int NOT NULL AUTO_INCREMENT,
  `session_id` int NOT NULL,
  `user_id` int NOT NULL,
  `status` enum('PRESENT','LATE','ABSENT','EXCUSED') COLLATE utf8mb4_unicode_ci NOT NULL,
  `check_in_time` datetime DEFAULT NULL,
  PRIMARY KEY (`record_id`),
  UNIQUE KEY `uq_session_student` (`session_id`,`user_id`),
  KEY `idx_records_student` (`user_id`),
  CONSTRAINT `fk_records_session` FOREIGN KEY (`session_id`) REFERENCES `attendance_sessions` (`session_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_records_student` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE RESTRICT
) ENGINE=InnoDB AUTO_INCREMENT=329 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `attendance_records`
--

LOCK TABLES `attendance_records` WRITE;
/*!40000 ALTER TABLE `attendance_records` DISABLE KEYS */;
INSERT INTO `attendance_records` VALUES (1,2,4,'LATE','2026-09-01 07:34:24'),(2,2,5,'LATE','2026-09-01 07:36:38'),(3,2,6,'EXCUSED',NULL),(4,2,7,'ABSENT',NULL),(5,2,8,'PRESENT','2026-09-01 07:11:11'),(6,2,9,'PRESENT','2026-09-01 07:55:13'),(7,2,10,'EXCUSED',NULL),(8,2,11,'PRESENT','2026-09-01 07:13:01'),(9,2,12,'PRESENT','2026-09-01 07:02:33'),(10,2,13,'PRESENT','2026-09-01 07:06:00'),(11,2,14,'ABSENT',NULL),(12,2,15,'PRESENT','2026-09-01 07:10:46'),(13,2,16,'LATE','2026-09-01 07:55:01'),(14,2,17,'PRESENT','2026-09-01 07:10:54'),(15,2,18,'PRESENT','2026-09-01 07:12:18'),(16,2,19,'PRESENT','2026-09-01 07:13:47'),(17,2,20,'ABSENT',NULL),(18,2,21,'EXCUSED',NULL),(19,2,22,'PRESENT','2026-09-01 07:13:04'),(20,2,23,'PRESENT','2026-09-01 07:17:54'),(21,3,4,'PRESENT','2026-09-02 13:19:26'),(22,3,5,'PRESENT','2026-09-02 13:09:14'),(23,3,6,'ABSENT',NULL),(24,3,7,'PRESENT','2026-09-02 13:00:07'),(25,3,8,'PRESENT','2026-09-02 13:07:38'),(26,3,9,'PRESENT','2026-09-02 13:15:57'),(27,3,10,'PRESENT','2026-09-02 13:13:44'),(28,3,11,'PRESENT','2026-09-02 13:08:41'),(29,3,12,'PRESENT','2026-09-02 13:02:26'),(30,3,13,'PRESENT','2026-09-02 13:04:15'),(31,3,14,'ABSENT',NULL),(32,3,15,'PRESENT','2026-09-02 13:05:56'),(33,3,16,'PRESENT','2026-09-02 13:03:04'),(34,3,17,'PRESENT','2026-09-02 13:04:55'),(35,3,18,'PRESENT','2026-09-02 13:10:48'),(36,4,6,'LATE','2026-09-02 15:30:26'),(37,4,7,'PRESENT','2026-09-02 15:07:56'),(38,4,8,'PRESENT','2026-09-02 15:16:41'),(39,4,9,'ABSENT',NULL),(40,4,10,'PRESENT','2026-09-02 15:09:00'),(41,4,11,'PRESENT','2026-09-02 15:10:26'),(42,4,12,'PRESENT','2026-09-02 15:09:21'),(43,4,13,'PRESENT','2026-09-02 15:57:28'),(44,4,14,'ABSENT',NULL),(45,4,15,'EXCUSED',NULL),(46,4,16,'LATE','2026-09-02 15:26:30'),(47,4,17,'PRESENT','2026-09-02 15:00:13'),(48,4,18,'ABSENT',NULL),(49,4,19,'LATE','2026-09-02 16:02:22'),(50,4,20,'PRESENT','2026-09-02 15:11:33'),(51,5,9,'PRESENT','2026-09-03 09:01:36'),(52,5,10,'LATE','2026-09-03 09:51:19'),(53,5,11,'PRESENT','2026-09-03 09:05:50'),(54,5,12,'PRESENT','2026-09-03 09:03:31'),(55,5,13,'EXCUSED',NULL),(56,5,14,'PRESENT','2026-09-03 09:15:46'),(57,5,15,'PRESENT','2026-09-03 09:10:49'),(58,5,16,'PRESENT','2026-09-03 09:09:37'),(59,5,17,'ABSENT',NULL),(60,5,18,'PRESENT','2026-09-03 09:16:17'),(61,5,19,'ABSENT',NULL),(62,5,20,'PRESENT','2026-09-03 09:05:41'),(63,5,21,'PRESENT','2026-09-03 09:01:21'),(64,5,22,'PRESENT','2026-09-03 09:11:17'),(65,5,23,'PRESENT','2026-09-03 09:07:06'),(66,6,4,'PRESENT','2026-09-07 10:15:28'),(67,6,5,'PRESENT','2026-09-07 10:07:56'),(68,6,8,'ABSENT',NULL),(69,6,9,'PRESENT','2026-09-07 10:01:31'),(70,6,10,'PRESENT','2026-09-07 10:04:35'),(71,6,11,'PRESENT','2026-09-07 10:15:59'),(72,6,12,'PRESENT','2026-09-07 10:10:10'),(73,6,13,'ABSENT',NULL),(74,6,14,'PRESENT','2026-09-07 10:00:41'),(75,6,15,'PRESENT','2026-09-07 10:02:31'),(76,6,16,'PRESENT','2026-09-07 10:16:34'),(77,7,4,'PRESENT','2026-09-15 07:00:08'),(78,7,5,'PRESENT','2026-09-15 07:11:23'),(79,7,6,'EXCUSED',NULL),(80,7,7,'PRESENT','2026-09-15 07:00:11'),(81,7,8,'PRESENT','2026-09-15 07:17:09'),(82,7,9,'PRESENT','2026-09-15 07:07:18'),(83,7,10,'PRESENT','2026-09-15 07:11:43'),(84,7,11,'PRESENT','2026-09-15 07:13:24'),(85,7,12,'PRESENT','2026-09-15 07:05:12'),(86,7,13,'ABSENT',NULL),(87,7,14,'PRESENT','2026-09-15 07:17:54'),(88,7,15,'PRESENT','2026-09-15 07:06:30'),(89,7,16,'PRESENT','2026-09-15 07:11:27'),(90,7,17,'PRESENT','2026-09-15 07:12:43'),(91,7,18,'ABSENT',NULL),(92,7,19,'PRESENT','2026-09-15 07:01:45'),(93,7,20,'PRESENT','2026-09-15 07:01:02'),(94,7,21,'PRESENT','2026-09-15 07:08:04'),(95,7,22,'EXCUSED',NULL),(96,7,23,'ABSENT',NULL),(97,8,4,'PRESENT','2026-09-16 13:08:00'),(98,8,5,'PRESENT','2026-09-16 13:19:43'),(99,8,6,'ABSENT',NULL),(100,8,7,'PRESENT','2026-09-16 13:00:55'),(101,8,8,'ABSENT',NULL),(102,8,9,'PRESENT','2026-09-16 13:04:00'),(103,8,10,'PRESENT','2026-09-16 13:12:49'),(104,8,11,'PRESENT','2026-09-16 13:04:06'),(105,8,12,'LATE','2026-09-16 13:23:44'),(106,8,13,'LATE','2026-09-16 13:58:13'),(107,8,14,'PRESENT','2026-09-16 13:06:25'),(108,8,15,'PRESENT','2026-09-16 13:10:28'),(109,8,16,'PRESENT','2026-09-16 13:05:16'),(110,8,17,'PRESENT','2026-09-16 13:04:12'),(111,8,18,'PRESENT','2026-09-16 13:14:40'),(112,9,6,'ABSENT',NULL),(113,9,7,'PRESENT','2026-09-16 15:12:10'),(114,9,8,'PRESENT','2026-09-16 15:09:49'),(115,9,9,'PRESENT','2026-09-16 15:01:07'),(116,9,10,'PRESENT','2026-09-16 15:11:04'),(117,9,11,'ABSENT',NULL),(118,9,12,'PRESENT','2026-09-16 15:11:24'),(119,9,13,'PRESENT','2026-09-16 15:10:05'),(120,9,14,'PRESENT','2026-09-16 15:14:38'),(121,9,15,'PRESENT','2026-09-16 15:01:43'),(122,9,16,'PRESENT','2026-09-16 15:10:14'),(123,9,17,'PRESENT','2026-09-16 15:03:10'),(124,9,18,'LATE','2026-09-16 15:41:53'),(125,9,19,'PRESENT','2026-09-16 15:14:32'),(126,9,20,'PRESENT','2026-09-16 15:16:40'),(127,10,9,'PRESENT','2026-09-17 09:01:15'),(128,10,10,'ABSENT',NULL),(129,10,11,'PRESENT','2026-09-17 09:01:29'),(130,10,12,'PRESENT','2026-09-17 09:08:17'),(131,10,13,'PRESENT','2026-09-17 09:00:20'),(132,10,14,'PRESENT','2026-09-17 09:08:34'),(133,10,15,'PRESENT','2026-09-17 09:00:25'),(134,10,16,'PRESENT','2026-09-17 09:02:34'),(135,10,17,'LATE','2026-09-17 09:46:17'),(136,10,18,'PRESENT','2026-09-17 09:17:47'),(137,10,19,'PRESENT','2026-09-17 09:01:00'),(138,10,20,'PRESENT','2026-09-17 09:17:55'),(139,10,21,'PRESENT','2026-09-17 09:15:39'),(140,10,22,'PRESENT','2026-09-17 09:05:38'),(141,10,23,'PRESENT','2026-09-17 09:19:57'),(142,11,4,'PRESENT','2026-09-28 10:12:57'),(143,11,5,'PRESENT','2026-09-28 10:03:29'),(144,11,8,'PRESENT','2026-09-28 10:07:35'),(145,11,9,'PRESENT','2026-09-28 10:16:33'),(146,11,10,'LATE','2026-09-28 10:24:17'),(147,11,11,'PRESENT','2026-09-28 10:00:16'),(148,11,12,'PRESENT','2026-09-28 10:19:55'),(149,11,13,'ABSENT',NULL),(150,11,14,'LATE','2026-09-28 10:23:13'),(151,11,15,'PRESENT','2026-09-28 10:17:57'),(152,11,16,'PRESENT','2026-09-28 10:00:06'),(153,12,4,'PRESENT','2026-09-29 08:01:23'),(154,12,5,'PRESENT','2026-09-29 07:07:51'),(155,12,6,'PRESENT','2026-09-29 07:19:54'),(156,12,7,'PRESENT','2026-09-29 07:26:38'),(157,12,8,'PRESENT','2026-09-29 07:06:36'),(158,12,9,'PRESENT','2026-09-29 07:09:21'),(159,12,10,'LATE','2026-09-29 07:48:45'),(160,12,11,'PRESENT','2026-09-29 07:18:30'),(161,12,12,'PRESENT','2026-09-29 07:01:40'),(162,12,13,'ABSENT',NULL),(163,12,14,'PRESENT','2026-09-29 07:02:00'),(164,12,15,'PRESENT','2026-09-29 07:08:17'),(165,12,16,'PRESENT','2026-09-29 07:17:01'),(166,12,17,'PRESENT','2026-09-29 07:09:33'),(167,12,18,'PRESENT','2026-09-29 07:01:22'),(168,12,19,'LATE','2026-09-29 07:34:38'),(169,12,20,'PRESENT','2026-09-29 07:10:30'),(170,12,21,'PRESENT','2026-09-29 07:07:50'),(171,12,22,'ABSENT',NULL),(172,12,23,'PRESENT','2026-09-29 07:03:15'),(173,13,4,'EXCUSED',NULL),(174,13,5,'PRESENT','2026-09-30 13:07:58'),(175,13,6,'PRESENT','2026-09-30 13:02:58'),(176,13,7,'PRESENT','2026-09-30 13:02:46'),(177,13,8,'EXCUSED',NULL),(178,13,9,'PRESENT','2026-09-30 13:02:43'),(179,13,10,'PRESENT','2026-09-30 13:05:33'),(180,13,11,'PRESENT','2026-09-30 13:13:55'),(181,13,12,'PRESENT','2026-09-30 13:19:36'),(182,13,13,'ABSENT',NULL),(183,13,14,'PRESENT','2026-09-30 13:09:05'),(184,13,15,'PRESENT','2026-09-30 13:14:06'),(185,13,16,'PRESENT','2026-09-30 13:09:17'),(186,13,17,'LATE','2026-09-30 13:23:16'),(187,13,18,'PRESENT','2026-09-30 13:13:00'),(188,14,6,'LATE','2026-09-30 16:01:01'),(189,14,7,'PRESENT','2026-09-30 15:02:47'),(190,14,8,'PRESENT','2026-09-30 15:02:43'),(191,14,9,'PRESENT','2026-09-30 15:03:17'),(192,14,10,'PRESENT','2026-09-30 15:12:52'),(193,14,11,'PRESENT','2026-09-30 15:13:05'),(194,14,12,'PRESENT','2026-09-30 15:04:23'),(195,14,13,'PRESENT','2026-09-30 15:17:31'),(196,14,14,'EXCUSED',NULL),(197,14,15,'PRESENT','2026-09-30 15:19:41'),(198,14,16,'LATE','2026-09-30 15:25:44'),(199,14,17,'PRESENT','2026-09-30 15:19:25'),(200,14,18,'LATE','2026-09-30 15:53:13'),(201,14,19,'PRESENT','2026-09-30 15:16:44'),(202,14,20,'LATE','2026-09-30 16:04:57'),(203,15,9,'ABSENT',NULL),(204,15,10,'PRESENT','2026-10-01 09:09:58'),(205,15,11,'PRESENT','2026-10-01 09:41:01'),(206,15,12,'PRESENT','2026-10-01 09:00:15'),(207,15,13,'LATE','2026-10-01 09:55:21'),(208,15,14,'PRESENT','2026-10-01 09:06:24'),(209,15,15,'LATE','2026-10-01 09:54:18'),(210,15,16,'PRESENT','2026-10-01 09:10:48'),(211,15,17,'PRESENT','2026-10-01 09:19:28'),(212,15,18,'PRESENT','2026-10-01 09:19:55'),(213,15,19,'PRESENT','2026-10-01 09:02:29'),(214,15,20,'PRESENT','2026-10-01 09:15:30'),(215,15,21,'PRESENT','2026-10-01 09:12:01'),(216,15,22,'PRESENT','2026-10-01 09:04:29'),(217,15,23,'PRESENT','2026-10-01 09:57:11'),(218,16,4,'PRESENT','2026-10-13 07:08:05'),(219,16,5,'LATE','2026-10-13 08:06:57'),(220,16,6,'PRESENT','2026-10-13 07:10:00'),(221,16,7,'ABSENT',NULL),(222,16,8,'EXCUSED',NULL),(223,16,9,'PRESENT','2026-10-13 07:00:49'),(224,16,10,'PRESENT','2026-10-13 07:16:53'),(225,16,11,'PRESENT','2026-10-13 07:05:04'),(226,16,12,'PRESENT','2026-10-13 07:15:45'),(227,16,13,'EXCUSED',NULL),(228,16,14,'EXCUSED',NULL),(229,16,15,'PRESENT','2026-10-13 07:01:48'),(230,16,16,'LATE','2026-10-13 08:03:46'),(231,16,17,'PRESENT','2026-10-13 07:09:58'),(232,16,18,'PRESENT','2026-10-13 07:06:35'),(233,16,19,'PRESENT','2026-10-13 07:07:57'),(234,16,20,'PRESENT','2026-10-13 07:06:28'),(235,16,21,'LATE','2026-10-13 08:05:51'),(236,16,22,'PRESENT','2026-10-13 07:16:59'),(237,16,23,'PRESENT','2026-10-13 07:17:27'),(238,17,4,'PRESENT','2026-10-14 13:09:44'),(239,17,5,'PRESENT','2026-10-14 13:13:23'),(240,17,6,'EXCUSED',NULL),(241,17,7,'ABSENT',NULL),(242,17,8,'PRESENT','2026-10-14 13:14:02'),(243,17,9,'PRESENT','2026-10-14 13:14:24'),(244,17,10,'PRESENT','2026-10-14 13:11:30'),(245,17,11,'LATE','2026-10-14 14:07:43'),(246,17,12,'PRESENT','2026-10-14 13:03:19'),(247,17,13,'ABSENT',NULL),(248,17,14,'ABSENT',NULL),(249,17,15,'LATE','2026-10-14 13:55:03'),(250,17,16,'PRESENT','2026-10-14 13:19:46'),(251,17,17,'PRESENT','2026-10-14 13:17:20'),(252,17,18,'PRESENT','2026-10-14 13:14:37'),(253,18,6,'PRESENT','2026-10-14 15:16:05'),(254,18,7,'PRESENT','2026-10-14 15:02:44'),(255,18,8,'PRESENT','2026-10-14 15:06:12'),(256,18,9,'PRESENT','2026-10-14 15:00:47'),(257,18,10,'PRESENT','2026-10-14 15:05:41'),(258,18,11,'ABSENT',NULL),(259,18,12,'PRESENT','2026-10-14 15:06:16'),(260,18,13,'ABSENT',NULL),(261,18,14,'LATE','2026-10-14 15:45:16'),(262,18,15,'PRESENT','2026-10-14 15:13:55'),(263,18,16,'PRESENT','2026-10-14 15:00:36'),(264,18,17,'PRESENT','2026-10-14 15:06:46'),(265,18,18,'PRESENT','2026-10-14 15:00:59'),(266,18,19,'ABSENT',NULL),(267,18,20,'PRESENT','2026-10-14 15:07:58'),(268,19,9,'PRESENT','2026-10-15 09:15:27'),(269,19,10,'PRESENT','2026-10-15 09:03:55'),(270,19,11,'PRESENT','2026-10-15 09:02:40'),(271,19,12,'PRESENT','2026-10-15 09:00:56'),(272,19,13,'EXCUSED',NULL),(273,19,14,'LATE','2026-10-15 10:09:13'),(274,19,15,'ABSENT',NULL),(275,19,16,'PRESENT','2026-10-15 09:11:29'),(276,19,17,'EXCUSED',NULL),(277,19,18,'PRESENT','2026-10-15 09:11:09'),(278,19,19,'ABSENT',NULL),(279,19,20,'PRESENT','2026-10-15 09:05:31'),(280,19,21,'PRESENT','2026-10-15 09:03:45'),(281,19,22,'PRESENT','2026-10-15 09:02:01'),(282,19,23,'PRESENT','2026-10-15 09:13:42'),(283,20,4,'PRESENT','2026-10-19 10:13:45'),(284,20,5,'EXCUSED',NULL),(285,20,8,'PRESENT','2026-10-19 10:04:00'),(286,20,9,'PRESENT','2026-10-19 10:16:42'),(287,20,10,'PRESENT','2026-10-19 10:09:42'),(288,20,11,'PRESENT','2026-10-19 10:07:14'),(289,20,12,'ABSENT',NULL),(290,20,13,'PRESENT','2026-10-19 10:14:17'),(291,20,14,'PRESENT','2026-10-19 10:04:18'),(292,20,15,'PRESENT','2026-10-19 10:12:55'),(293,20,16,'ABSENT',NULL),(294,21,4,'ABSENT',NULL),(295,21,5,'PRESENT','2026-10-27 07:13:23'),(296,21,6,'PRESENT','2026-10-27 07:16:54'),(297,21,7,'PRESENT','2026-10-27 07:13:09'),(298,21,8,'PRESENT','2026-10-27 07:00:47'),(299,21,9,'PRESENT','2026-10-27 07:11:14'),(300,21,10,'PRESENT','2026-10-27 07:05:48'),(301,21,11,'PRESENT','2026-10-27 07:06:01'),(302,21,12,'PRESENT','2026-10-27 07:02:23'),(303,21,13,'LATE','2026-10-27 07:55:49'),(304,21,14,'PRESENT','2026-10-27 07:15:52'),(305,21,15,'PRESENT','2026-10-27 07:12:12'),(306,21,16,'ABSENT',NULL),(307,21,17,'PRESENT','2026-10-27 07:18:59'),(308,21,18,'LATE','2026-10-27 07:47:11'),(309,21,19,'PRESENT','2026-10-27 07:16:25'),(310,21,20,'LATE','2026-10-27 07:30:23'),(311,21,21,'PRESENT','2026-10-27 07:14:07'),(312,21,22,'PRESENT','2026-10-27 07:03:07'),(313,21,23,'PRESENT','2026-10-27 07:18:13'),(314,22,4,'PRESENT','2026-10-28 13:00:25'),(315,22,5,'PRESENT','2026-10-28 13:08:50'),(316,22,6,'PRESENT','2026-10-28 13:09:06'),(317,22,7,'EXCUSED',NULL),(318,22,8,'ABSENT',NULL),(319,22,9,'PRESENT','2026-10-28 13:06:40'),(320,22,10,'LATE','2026-10-28 13:22:23'),(321,22,11,'PRESENT','2026-10-28 13:15:33'),(322,22,12,'ABSENT',NULL),(323,22,13,'PRESENT','2026-10-28 13:06:20'),(324,22,14,'PRESENT','2026-10-28 13:17:22'),(325,22,15,'PRESENT','2026-10-28 13:05:55'),(326,22,16,'PRESENT','2026-10-28 13:13:38'),(327,22,17,'PRESENT','2026-10-28 13:18:31'),(328,22,18,'PRESENT','2026-10-28 13:13:25');
/*!40000 ALTER TABLE `attendance_records` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `attendance_sessions`
--

DROP TABLE IF EXISTS `attendance_sessions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `attendance_sessions` (
  `session_id` int NOT NULL AUTO_INCREMENT,
  `course_id` int NOT NULL,
  `user_id` int NOT NULL,
  `start_time` datetime NOT NULL,
  `duration_minutes` int NOT NULL,
  `present_window_minutes` int NOT NULL DEFAULT '20',
  `late_window_minutes` int NOT NULL DEFAULT '80',
  `session_code` varchar(10) COLLATE utf8mb4_unicode_ci NOT NULL,
  `code_generated_at` datetime NOT NULL,
  `status` enum('OPEN','CLOSED') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'OPEN',
  `closed_at` datetime DEFAULT NULL,
  PRIMARY KEY (`session_id`),
  KEY `fk_sessions_lecturer` (`user_id`),
  KEY `idx_sessions_course_status` (`course_id`,`status`),
  CONSTRAINT `fk_sessions_course` FOREIGN KEY (`course_id`) REFERENCES `courses` (`course_id`) ON DELETE RESTRICT,
  CONSTRAINT `fk_sessions_lecturer` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE RESTRICT,
  CONSTRAINT `chk_session_windows` CHECK (((`present_window_minutes` > 0) and (`present_window_minutes` <= `late_window_minutes`) and (`late_window_minutes` <= `duration_minutes`)))
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `attendance_sessions`
--

LOCK TABLES `attendance_sessions` WRITE;
/*!40000 ALTER TABLE `attendance_sessions` DISABLE KEYS */;
INSERT INTO `attendance_sessions` VALUES (1,1,2,'2026-11-15 15:56:23',80,20,80,'528419','2026-11-15 15:56:23','OPEN',NULL),(2,3,3,'2026-09-01 07:00:00',150,20,80,'320939','2026-09-01 07:00:00','CLOSED','2026-09-01 09:30:00'),(3,1,2,'2026-09-02 13:00:00',90,20,80,'921193','2026-09-02 13:00:00','CLOSED','2026-09-02 14:30:00'),(4,4,2,'2026-09-02 15:00:00',150,20,80,'424522','2026-09-02 15:00:00','CLOSED','2026-09-02 17:30:00'),(5,5,3,'2026-09-03 09:00:00',150,20,80,'436231','2026-09-03 09:00:00','CLOSED','2026-09-03 11:30:00'),(6,2,3,'2026-09-07 10:00:00',90,20,80,'847716','2026-09-07 10:00:00','CLOSED','2026-09-07 11:30:00'),(7,3,3,'2026-09-15 07:00:00',150,20,80,'199466','2026-09-15 07:00:00','CLOSED','2026-09-15 09:30:00'),(8,1,2,'2026-09-16 13:00:00',90,20,80,'469807','2026-09-16 13:00:00','CLOSED','2026-09-16 14:30:00'),(9,4,2,'2026-09-16 15:00:00',150,20,80,'840249','2026-09-16 15:00:00','CLOSED','2026-09-16 17:30:00'),(10,5,3,'2026-09-17 09:00:00',150,20,80,'501246','2026-09-17 09:00:00','CLOSED','2026-09-17 11:30:00'),(11,2,3,'2026-09-28 10:00:00',90,20,80,'475569','2026-09-28 10:00:00','CLOSED','2026-09-28 11:30:00'),(12,3,3,'2026-09-29 07:00:00',150,20,80,'830793','2026-09-29 07:00:00','CLOSED','2026-09-29 09:30:00'),(13,1,2,'2026-09-30 13:00:00',90,20,80,'128195','2026-09-30 13:00:00','CLOSED','2026-09-30 14:30:00'),(14,4,2,'2026-09-30 15:00:00',150,20,80,'843249','2026-09-30 15:00:00','CLOSED','2026-09-30 17:30:00'),(15,5,3,'2026-10-01 09:00:00',150,20,80,'893227','2026-10-01 09:00:00','CLOSED','2026-10-01 11:30:00'),(16,3,3,'2026-10-13 07:00:00',150,20,80,'443953','2026-10-13 07:00:00','CLOSED','2026-10-13 09:30:00'),(17,1,2,'2026-10-14 13:00:00',90,20,80,'612802','2026-10-14 13:00:00','CLOSED','2026-10-14 14:30:00'),(18,4,2,'2026-10-14 15:00:00',150,20,80,'639219','2026-10-14 15:00:00','CLOSED','2026-10-14 17:30:00'),(19,5,3,'2026-10-15 09:00:00',150,20,80,'154964','2026-10-15 09:00:00','CLOSED','2026-10-15 11:30:00'),(20,2,3,'2026-10-19 10:00:00',90,20,80,'684384','2026-10-19 10:00:00','CLOSED','2026-10-19 11:30:00'),(21,3,3,'2026-10-27 07:00:00',150,20,80,'689121','2026-10-27 07:00:00','CLOSED','2026-10-27 09:30:00'),(22,1,2,'2026-10-28 13:00:00',90,20,80,'132627','2026-10-28 13:00:00','CLOSED','2026-10-28 14:30:00');
/*!40000 ALTER TABLE `attendance_sessions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `courses`
--

DROP TABLE IF EXISTS `courses`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `courses` (
  `course_id` int NOT NULL AUTO_INCREMENT,
  `code` varchar(20) COLLATE utf8mb4_unicode_ci NOT NULL,
  `name` varchar(150) COLLATE utf8mb4_unicode_ci NOT NULL,
  `user_id` int NOT NULL,
  PRIMARY KEY (`course_id`),
  UNIQUE KEY `code` (`code`),
  KEY `idx_courses_lecturer` (`user_id`),
  CONSTRAINT `fk_courses_lecturer` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE RESTRICT
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `courses`
--

LOCK TABLES `courses` WRITE;
/*!40000 ALTER TABLE `courses` DISABLE KEYS */;
INSERT INTO `courses` VALUES (1,'CE201-01','Transport Planning',2),(2,'MA201-02','Applied Mathematics',3),(3,'IT301-01','Công nghệ phần mềm',3),(4,'IT202-02','Lập trình C++',2),(5,'IT204-01','Cơ sở dữ liệu',3);
/*!40000 ALTER TABLE `courses` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `enrollments`
--

DROP TABLE IF EXISTS `enrollments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `enrollments` (
  `enrollment_id` int NOT NULL AUTO_INCREMENT,
  `user_id` int NOT NULL,
  `course_id` int NOT NULL,
  `enrolled_at` date NOT NULL,
  PRIMARY KEY (`enrollment_id`),
  UNIQUE KEY `uq_enrollment` (`user_id`,`course_id`),
  KEY `fk_enrollments_course` (`course_id`),
  KEY `idx_enrollments_student` (`user_id`),
  CONSTRAINT `fk_enrollments_course` FOREIGN KEY (`course_id`) REFERENCES `courses` (`course_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_enrollments_student` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE RESTRICT
) ENGINE=InnoDB AUTO_INCREMENT=77 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `enrollments`
--

LOCK TABLES `enrollments` WRITE;
/*!40000 ALTER TABLE `enrollments` DISABLE KEYS */;
INSERT INTO `enrollments` VALUES (1,4,1,'2026-09-01'),(2,5,1,'2026-09-01'),(3,6,1,'2026-09-01'),(4,7,1,'2026-09-01'),(5,4,2,'2026-09-01'),(6,5,2,'2026-09-01'),(7,8,1,'2026-09-01'),(8,8,2,'2026-09-01'),(9,9,1,'2026-09-01'),(10,10,1,'2026-09-01'),(11,11,1,'2026-09-01'),(12,12,1,'2026-09-01'),(13,13,1,'2026-09-01'),(14,14,1,'2026-09-01'),(15,15,1,'2026-09-01'),(16,16,1,'2026-09-01'),(17,17,1,'2026-09-01'),(18,18,1,'2026-09-01'),(19,9,2,'2026-09-01'),(20,10,2,'2026-09-01'),(21,11,2,'2026-09-01'),(22,12,2,'2026-09-01'),(23,13,2,'2026-09-01'),(24,14,2,'2026-09-01'),(25,15,2,'2026-09-01'),(26,16,2,'2026-09-01'),(27,4,3,'2026-09-01'),(28,5,3,'2026-09-01'),(29,6,3,'2026-09-01'),(30,7,3,'2026-09-01'),(31,8,3,'2026-09-01'),(32,9,3,'2026-09-01'),(33,10,3,'2026-09-01'),(34,11,3,'2026-09-01'),(35,12,3,'2026-09-01'),(36,13,3,'2026-09-01'),(37,14,3,'2026-09-01'),(38,15,3,'2026-09-01'),(39,16,3,'2026-09-01'),(40,17,3,'2026-09-01'),(41,18,3,'2026-09-01'),(42,19,3,'2026-09-01'),(43,20,3,'2026-09-01'),(44,21,3,'2026-09-01'),(45,22,3,'2026-09-01'),(46,23,3,'2026-09-01'),(47,6,4,'2026-09-01'),(48,7,4,'2026-09-01'),(49,8,4,'2026-09-01'),(50,9,4,'2026-09-01'),(51,10,4,'2026-09-01'),(52,11,4,'2026-09-01'),(53,12,4,'2026-09-01'),(54,13,4,'2026-09-01'),(55,14,4,'2026-09-01'),(56,15,4,'2026-09-01'),(57,16,4,'2026-09-01'),(58,17,4,'2026-09-01'),(59,18,4,'2026-09-01'),(60,19,4,'2026-09-01'),(61,20,4,'2026-09-01'),(62,9,5,'2026-09-01'),(63,10,5,'2026-09-01'),(64,11,5,'2026-09-01'),(65,12,5,'2026-09-01'),(66,13,5,'2026-09-01'),(67,14,5,'2026-09-01'),(68,15,5,'2026-09-01'),(69,16,5,'2026-09-01'),(70,17,5,'2026-09-01'),(71,18,5,'2026-09-01'),(72,19,5,'2026-09-01'),(73,20,5,'2026-09-01'),(74,21,5,'2026-09-01'),(75,22,5,'2026-09-01'),(76,23,5,'2026-09-01');
/*!40000 ALTER TABLE `enrollments` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `schedule_entries`
--

DROP TABLE IF EXISTS `schedule_entries`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `schedule_entries` (
  `schedule_id` int NOT NULL AUTO_INCREMENT,
  `course_id` int NOT NULL,
  `type` enum('CLASS','STUDY') COLLATE utf8mb4_unicode_ci NOT NULL,
  `day_of_week` tinyint NOT NULL,
  `start_time` time NOT NULL,
  `end_time` time NOT NULL,
  `room` varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL,
  `start_date` date NOT NULL,
  `end_date` date NOT NULL,
  PRIMARY KEY (`schedule_id`),
  KEY `idx_schedules_course_date` (`course_id`,`start_date`,`end_date`),
  CONSTRAINT `fk_schedules_course` FOREIGN KEY (`course_id`) REFERENCES `courses` (`course_id`) ON DELETE CASCADE,
  CONSTRAINT `chk_schedule_rules` CHECK (((`day_of_week` between 1 and 7) and (`start_time` < `end_time`) and (`start_date` <= `end_date`)))
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `schedule_entries`
--

LOCK TABLES `schedule_entries` WRITE;
/*!40000 ALTER TABLE `schedule_entries` DISABLE KEYS */;
INSERT INTO `schedule_entries` VALUES (1,1,'CLASS',1,'08:00:00','09:30:00','B.402','2026-09-01','2026-12-30'),(2,1,'CLASS',3,'13:00:00','14:30:00','B.405','2026-09-01','2026-12-30'),(3,2,'CLASS',1,'10:00:00','11:30:00','A.203','2026-09-01','2026-12-30'),(4,3,'CLASS',2,'07:00:00','09:30:00','C.301','2026-09-01','2026-12-30'),(5,3,'CLASS',5,'13:00:00','15:30:00','C.301','2026-09-01','2026-12-30'),(6,4,'CLASS',3,'15:00:00','17:30:00','LAB.2','2026-09-01','2026-12-30'),(7,4,'STUDY',6,'08:00:00','10:00:00','LAB.2','2026-09-01','2026-12-30'),(8,5,'CLASS',4,'09:00:00','11:30:00','A.105','2026-09-01','2026-12-30');
/*!40000 ALTER TABLE `schedule_entries` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `users` (
  `user_id` int NOT NULL AUTO_INCREMENT,
  `role` enum('STUDENT','LECTURER','ADMIN') COLLATE utf8mb4_unicode_ci NOT NULL,
  `code` varchar(20) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `full_name` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,
  `password_hash` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `is_active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `email` (`email`),
  UNIQUE KEY `code` (`code`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'ADMIN',NULL,'Lê Hoàng Phúc','admin@uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(2,'LECTURER','GV0042','Dr. Trần Minh Quân','quan.tran@uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(3,'LECTURER','GV0057','Nguyễn Thu Hà','ha.nguyen@uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(4,'STUDENT','2451010001','Nguyễn Minh Anh','minhanh.nguyen@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(5,'STUDENT','2451010002','Phạm Quốc Bảo','bao.pham@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(6,'STUDENT','2451010003','Võ Tấn Lộc','loc.vo@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(7,'STUDENT','2451010004','Trần Thảo Vy','vy.tran@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(8,'STUDENT','2451010005','Đặng Gia Hân','han.dang@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(9,'STUDENT','2451010006','Lê Hoàng Nam','nam.le@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(10,'STUDENT','2451010007','Bùi Thị Mai','mai.bui@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(11,'STUDENT','2451010008','Hoàng Đức Thịnh','thinh.hoang@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(12,'STUDENT','2451010009','Ngô Khánh Linh','linh.ngo@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(13,'STUDENT','2451010010','Đỗ Anh Tuấn','tuan.do@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(14,'STUDENT','2451010011','Phan Thị Hương','huong.phan@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(15,'STUDENT','2451010012','Vũ Quang Huy','huy.vu@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(16,'STUDENT','2451010013','Dương Ngọc Ánh','anh.duong@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(17,'STUDENT','2451010014','Lý Gia Bảo','bao.ly@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(18,'STUDENT','2451010015','Trương Mỹ Duyên','duyen.truong@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(19,'STUDENT','2451010016','Huỳnh Minh Khôi','khoi.huynh@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(20,'STUDENT','2451010017','Đinh Thu Hà','ha.dinh@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(21,'STUDENT','2451010018','Lâm Hoàng Sơn','son.lam@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(22,'STUDENT','2451010019','Mai Phương Thảo','thao.mai@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23'),(23,'STUDENT','2451010020','Cao Tuấn Kiệt','kiet.cao@sv.uth.edu.vn','e10adc3949ba59abbe56e057f20f883e',1,'2026-11-15 15:56:23');
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping routines for database 'student_attendance'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-11-16  2:37:52
