#
# Dumping data for table `CompatTypes`
#

INSERT INTO CompatTypes VALUES ('','Not tested','icons/nt.gif');
INSERT INTO CompatTypes VALUES (10,'Not supported by application','icons/blank.gif');
INSERT INTO CompatTypes VALUES (20,'Works very well','icons/w3.gif');
INSERT INTO CompatTypes VALUES (50,'Works (minor problems)','icons/w1.gif');
INSERT INTO CompatTypes VALUES (55,'Works (major problems)','icons/w0.gif');
INSERT INTO CompatTypes VALUES (90,'Does not work','icons/nw.gif');

#
# Dumping data for table `DistributionTypes`
#

INSERT INTO DistributionTypes VALUES ('','Unspecified distribution media type','icons/blank.gif');
INSERT INTO DistributionTypes VALUES (100,'Floppy disk distribution','icons/floppy.gif');
INSERT INTO DistributionTypes VALUES (200,'CD-ROM/DVD distribution','icons/cd.gif');

#
# Dumping data for table `EMUVerTypes`
#

INSERT INTO EMUVerTypes VALUES ('','Native support','speaker','icons/blank.gif');
INSERT INTO EMUVerTypes VALUES (100,'VDMSound 1.1','midi,sb','icons/vdms1.gif');
INSERT INTO EMUVerTypes VALUES (101,'VDMSound 2.0.3','midi,sb,joystick','icons/vdms2.gif');
INSERT INTO EMUVerTypes VALUES (102,'VDMSound 2.0.4','midi,sb,adlib,joystick','icons/vdms2.gif');
INSERT INTO EMUVerTypes VALUES (200,'SoundFX 2.00','midi,sb,adlib,joystick','icons/soundfx2k.gif');
INSERT INTO EMUVerTypes VALUES (201,'SoundFX 2.01','midi,sb,adlib,joystick','icons/soundfx2k.gif');
INSERT INTO EMUVerTypes VALUES (202,'SoundFX 2.02','midi,sb,adlib,joystick','icons/soundfx2k.gif');
INSERT INTO EMUVerTypes VALUES (300,'GUSEmu 0.80','gus','icons/blank.gif');
INSERT INTO EMUVerTypes VALUES (301,'GUSEmu 0.84','gus','icons/blank.gif');
INSERT INTO EMUVerTypes VALUES (500,'DosBox 0.1','speaker,sb,adlib','icons/blank.gif');
INSERT INTO EMUVerTypes VALUES (501,'DosBox 0.2','speaker,sb,adlib','icons/blank.gif');

#
# Dumping data for table `OSVerTypes`
#

INSERT INTO OSVerTypes VALUES ('','Unspecified OS','icons/blank.gif');
INSERT INTO OSVerTypes VALUES (1000,'DOS 5.0','icons/dos.gif');
INSERT INTO OSVerTypes VALUES (1001,'DOS 6.0','icons/dos.gif');
INSERT INTO OSVerTypes VALUES (1002,'DOS 6.2','icons/dos.gif');
INSERT INTO OSVerTypes VALUES (1003,'DOS 7.x','icons/dos.gif');
INSERT INTO OSVerTypes VALUES (1004,'DOS 8.x','icons/dos.gif');
INSERT INTO OSVerTypes VALUES (2010,'Windows 3.1','icons/win3x.gif');
INSERT INTO OSVerTypes VALUES (2011,'Windows 3.11','icons/win3x.gif');
INSERT INTO OSVerTypes VALUES (2100,'Windows 95','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2101,'Windows 95 SP1','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2102,'Windows 95A (OSR1)','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2103,'Windows 95B (OSR2/2.1)','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2104,'Windows 95C (OSR2.5)','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2110,'Windows 98','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2111,'Windows 98 SE','icons/win9x.gif');
INSERT INTO OSVerTypes VALUES (2120,'Windows ME','icons/winme.gif');
INSERT INTO OSVerTypes VALUES (2200,'Windows NT 4.0','icons/winnt.gif');
INSERT INTO OSVerTypes VALUES (2205,'Windows NT 4.0 SP5','icons/winnt.gif');
INSERT INTO OSVerTypes VALUES (2210,'Windows 2000','icons/win2k.gif');
INSERT INTO OSVerTypes VALUES (2211,'Windows 2000 SP1','icons/win2k.gif');
INSERT INTO OSVerTypes VALUES (2212,'Windows 2000 SP2','icons/win2k.gif');
INSERT INTO OSVerTypes VALUES (2220,'Windows XP','icons/winxp.gif');
INSERT INTO OSVerTypes VALUES (2221,'Windows XP SP1','icons/winxp.gif');

#
# Dumping data for table `News`
#

INSERT INTO News VALUES (1,'2002-02-24 08:06:24',20020224080624,'Finally...','Started work on the new Compatibility Database.');
INSERT INTO News VALUES (2,'2002-03-03 10:09:33',20020309120849,'Progress','Finished and tested user authentication, user profile editing, user registration and password reset.\r\nUploaded basic list of games from http://www.mobygames.com/ .');
INSERT INTO News VALUES (3,'2002-03-10 12:12:55',20020310121255,'More progress','HTML layout pretty stable.');
INSERT INTO News VALUES (4,'2002-03-17 15:02:11',20020317150211,'Almost there','Added report creation page, but not fully functional yet.');
INSERT INTO News VALUES (5,'2002-03-24 10:00:00',20020324100000,'Ready for test-drive','Everything is pretty much functional and ready for beta-testing.  DATA MAY BE DISCARDED, so do not spend too much time on reporting all your games, just do the minimum necessary for testing.');
