CREATE TABLE News (
	id INTEGER AUTO_INCREMENT NOT NULL,
	time_created DATETIME NOT NULL,
	time_updated TIMESTAMP DEFAULT NULL,
	title VARCHAR ( 255 ) NOT NULL,
	comment TEXT NOT NULL,
	PRIMARY KEY (id)
	);

CREATE TABLE DistributionTypes (
	id INTEGER NOT NULL,
	description VARCHAR ( 64 ) NOT NULL,
	icon_URL VARCHAR ( 255 ),
	PRIMARY KEY (id)
	);

CREATE TABLE Titles (
	id INTEGER AUTO_INCREMENT NOT NULL,
	name VARCHAR ( 255 ) NOT NULL,
	nameSoundex VARCHAR ( 255 ) NOT NULL,
	PRIMARY KEY (id),
	UNIQUE KEY name (name),
	FULLTEXT name_ft (name,nameSoundex)
	);

CREATE TABLE Applications (
	id INTEGER AUTO_INCREMENT NOT NULL,
	Titles_id INTEGER NOT NULL,
	version varchar ( 32 ),
	time_created DATETIME NOT NULL,
	time_updated TIMESTAMP DEFAULT NULL,
	DistributionTypes_id INTEGER NOT NULL,
	PRIMARY KEY (id)
	);

CREATE TABLE Users (
	id INTEGER AUTO_INCREMENT NOT NULL,
	name VARCHAR ( 12 ) NOT NULL,
	type SMALLINT NOT NULL,
	password CHAR ( 32 ) NOT NULL,
	time_created DATETIME NOT NULL,
	time_updated TIMESTAMP DEFAULT NULL,
	time_lastlogin DATETIME NOT NULL,
	fullname VARCHAR ( 48 ) NOT NULL,
	email VARCHAR ( 64 ) NOT NULL,
	salt INTEGER NOT NULL,
	PRIMARY KEY (id),
	UNIQUE KEY name (name)
	);

CREATE TABLE OSVerTypes (
	id INTEGER NOT NULL,
	description VARCHAR ( 48 ) NOT NULL,
	icon_URL VARCHAR ( 255 ),
	PRIMARY KEY (id)
	);

CREATE TABLE EMUVerTypes (
	id INTEGER NOT NULL,
	description VARCHAR ( 48 ) NOT NULL,
	icon_URL VARCHAR ( 255 ),
	PRIMARY KEY (id)
	);

CREATE TABLE CompatTypes (
	id SMALLINT NOT NULL,
	description VARCHAR ( 64 ) NOT NULL,
	icon_URL VARCHAR ( 255 ),
	PRIMARY KEY (id)
	);

CREATE TABLE Reports (
	id INTEGER AUTO_INCREMENT NOT NULL,
	Users_id INTEGER NOT NULL,
	Applications_id INTEGER NOT NULL,
	time_created DATETIME NOT NULL,
	time_updated TIMESTAMP DEFAULT NULL,
	comment TEXT,
	CompatTypes_id_midi SMALLINT NOT NULL,
	CompatTypes_id_sb SMALLINT NOT NULL,
	CompatTypes_id_adlib SMALLINT NOT NULL,
	CompatTypes_id_joystick SMALLINT NOT NULL,
	OSVerTypes_id INTEGER NOT NULL,
	EMUVerTypes_id INTEGER NOT NULL,
	PRIMARY KEY (id),
	KEY Users_id (Users_id),
	KEY Applications_id (Applications_id)
	);
