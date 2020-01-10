IGMP Protocol by Igor Schittekat and Wannes Maryen
==================================================

This is our project for the course Telecommunication Systems on the University of Antwerp. 

Contents for IGMP
-----------------

File                                | Description
----------------------------------- | -------------------------------
`IGMPDIR/IGMP_Packet`               | Packet forms
`IGMPDIR/IgmpClassifier`            | Splits IGMP packets from Multicast packets and other packets
`IGMPDIR/IGMPClientResponder`       | Client responds to messages sent from the router
`IGMPDIR/IgmpForwarder`             | Forwards multicast traffic to the client
`IGMPDIR/IgmpRouter`                | Router element
`IGMPDIR/IgmpRouterChecker`         | Verifies incomming traffic in the Router
`IGMPDIR/MembershipReportSender`    | Controls the handlers for join and leave messages
`IGMPDIR/RouterGeneralQuerySender`  | Periodically sends General Query Messages
`IGMPDIR/SetIGMPChecksum`           | Set the checksum of the IGMP packet
`LIBRARYDIR/client.click`           | click script for the client
`LIBRARYDIR/router.click`           | click script for the router
`LIBRARYDIR/server.click`           | click script for the server
`LIBRARYDIR/definitions.click`      | click script with used addresses


Handlers
-------------

A user can request to listen to a multicast address by joining that group. 
`write clientXX/igmp.join [multicast address]`
A user can also request to stop listening to a multicast address by leaving that group.
`write clientXX/igmp.leave [multicast address]`

