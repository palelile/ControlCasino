SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;


CREATE TABLE IF NOT EXISTS `alumnos` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `rut` int(8) DEFAULT NULL,
  `nombre` varchar(30) DEFAULT NULL,
  `apellidoP` varchar(13) DEFAULT NULL,
  `apellidoM` varchar(12) DEFAULT NULL,
  `curso` varchar(39) DEFAULT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

CREATE TABLE IF NOT EXISTS `asistenciaCasinoAlmuerzo` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `rut` text COLLATE utf8_spanish_ci NOT NULL,
  `fecha` text COLLATE utf8_spanish_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_spanish_ci AUTO_INCREMENT=1 ;

CREATE TABLE IF NOT EXISTS `asistenciaCasinoDesayuno` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `rut` text COLLATE utf8_spanish_ci NOT NULL,
  `fecha` text COLLATE utf8_spanish_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_spanish_ci AUTO_INCREMENT=1 ;

CREATE TABLE IF NOT EXISTS `autorizadosCasino` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `rut` text COLLATE utf8_spanish_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_spanish_ci AUTO_INCREMENT=1 ;

DROP TABLE `autorizadosCasino`;
DROP TABLE `alumnos`;
DROP TABLE `asistenciaCasinoAlmuerzo`;
DROP TABLE `asistenciaCasinoDesayuno`;