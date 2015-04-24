create database git;

show databases;

CREATE USER 'git'@'localhost' IDENTIFIED BY 'git';

GRANT ALL PRIVILEGES ON git.* TO 'git'@'localhost';

use git;