# CITRUS
NYP AAT 2022 route A3 FYP - Coolant Intuitive Top-up ReUsable System, dubbed CITRUS.

CITRUS's code is made up of two components, Arduino and PHP, using LAMP/WAMP. To use, clone this repository into your web server's DocumentRoot, and move the Arduino code to a place of your liking. Upload Arduino/sibeliusProto.ino to the Arduino, and create a SQL database according to DBformat.jpg. 

Important things to note is that you need to change the IP address in sibeliusProto/interface.cpp, to the private IP of the computer running the server. We had the server connected to a switch with an Ethernet cable. You also need to change the details of your database in public_html/config/sqlConfig.php accordingly. The login details in login.php are NOT the database details. They are in public_html/config/loginConfig.php, which you can choose. If this CITRUS folder is not in DocumentRoot, you may need to change the pathing in the PHP code. 

Configuration-wise, everything should work out of the box. Ideally, set up your server so that unauthorised users can't just directly go to editDB.php.

Arduino/old_versions simply contain old versions, in case anyone wants to replicate our system and do troubleshooting. beethovenV4 is a version that is pure Arduino with no database. You can test the pumps & valves with it. bachV7 has a database, but does not depend on a server. It relies on a Excel spreadsheet and macro, which communicates with the Arduino via serial. 
