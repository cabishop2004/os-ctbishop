This is how the program works in the terminal : echo "This is the original secure message." > message.txt
ctbishop@Caleb-School:~/CSCI470/security$ ./hmac_demo create message.txt
Authentication tag created.
File: message.txt
Tag saved to: message.tag
Tag: 038f9de33571213fe28be20f90f822a2b227a253f034dd03e2421ffe7ddcc4b2
ctbishop@Caleb-School:~/CSCI470/security$ ./hmac_demo verify message.txt
Saved tag:      038f9de33571213fe28be20f90f822a2b227a253f034dd03e2421ffe7ddcc4b2
Calculated tag: 038f9de33571213fe28be20f90f822a2b227a253f034dd03e2421ffe7ddcc4b2

Verification successful.
File has not been changed.
ctbishop@Caleb-School:~/CSCI470/security$ echo "An attacker changed this file." >> message.txt
ctbishop@Caleb-School:~/CSCI470/security$ ./hmac_demo verify message.txt
Saved tag:      038f9de33571213fe28be20f90f822a2b227a253f034dd03e2421ffe7ddcc4b2
Calculated tag: 211095168230609aa81e3e6272cd53cb2e3e41c2903ac3d4a9e01ae7cf4f7540

Verification failed.
File may have been modified.
ctbishop@Caleb-School:~/CSCI470/security$ 

