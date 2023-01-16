# CITRUS
NYP AAT 2022 route A3 FYP - Coolant Integrated Top-up Reliable United System, dubbed CITRUS.

CITRUS is made up of two components, and developed on LAMP/WAMP. Upload the code in sibeliusProto to the Arduino, and have the code in public_html run by an Apache webserver. The PHP code interfaces with a MySQL database, which has a format shown in format.jpeg. You will also need to have PHP on your computer, of course.

Important things to note is that you need to change the IP address in sibeliusProto/interface.cpp, to the private IP of the computer running the server. We had the server connected to a switch with an Ethernet cable. You also need to change the details of your database in public_html/config/sqlConfig.php accordingly. If you put the contents of public_html in their own folder instead of the DocumentRoot, you may need to change the pathing in the PHP code. The login details in loginpage.php are NOT the database details. They are in public_html/config/loginConfig.php, which you can choose.

Configuration-wise, everything should work out of the box. Ideally, prevent directory browsing in the server settings so that unauthorised users can't simply skip to the edit page. 

old_versions simply contain old versions, in case anyone wants to replicate our system and do troubleshooting. beethovenV4 is a version that is pure Arduino with no database. You can test the pumps & valves with it. bachV7 is not dependent on a server for a database. It relies on a Excel spreadsheet and macro, which communicates with the Arduino via serial. 

Disclaimer - The code might not be optimal, since we wrote it not having taught the languages themselves. The PHP code was not extensively tested for bugs, thus funny login names etc. might cause the pages to fail.

The PHP code was written by two of us then merged, so there might be a drastic change in coding style, and functions that do similar things. Most of the Arduino code in void loop() is written by me to optimise loop speed(i.e. responsiveness). Due to this, there might be some odd pieces of code, but they should be commented well enough.
