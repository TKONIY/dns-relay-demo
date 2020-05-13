<<<<<<< HEAD
# DNS-Relay ¡ª¡ª DNSÖÐ¼ÌÆ÷
## 1. ¿ò¼Ü
![Á÷³ÌÍ¼](./images/design.svg)
## 2. ±àÂë¹æ·¶
* ±äÁ¿Ãû: `yourFather`
* º¯ÊýÃû: `YourFather`
* ÎÄ¼þÃû: `your_father`
* ²»ÒªÔÚ`.h`ÎÄ¼þÀïÃæÉùÃ÷ÈÎºÎ±äÁ¿
* ½á¹¹ÌåÉùÃ÷·ÅÔÚ`.h`ÖÐ
* ËùÓÐµÄÈ«¾Ö±äÁ¿¶¼ÓÃ`static`ÐÞÊÎ,ÐèÒª·ÃÎÊµÄ»°Ìá¹©½Ó¿Ú
* ²»ÐèÒª±»Íâ²¿µ÷ÓÃµÄº¯Êý²»ÓÃÔÚ`.h`ÖÐÉùÃ÷,Ö»ÐèÒªÔÚ`.cpp`ÖÐÓÃ`static`ÐÞÊÎ¶¨Òå
* ËùÓÐµÄÍâ²¿º¯Êý(±»ÆäËûÄ£¿éµÄµ÷ÓÃµÄº¯Êý)±ØÐë¼Ó`extern`,(ËäÈ»Õâ¸ö`extern`Ã»ÓÐÊ²Ã´ÒâÒå);
* ËùÓÐ±íÊ¾Ò»¸ö×Ö½ÚµÄÄÚ´æÇø±ØÐëÊ¹ÓÃ`unsigned char`
* `include`È«²¿·ÅÔÚ`.h`ÎÄ¼þ°É,·ÀÖ¹ÖØ¸´¶¨Òå
* ×¢ÊÍÍ³Ò»Ê¹ÓÃ `/* ... */` ²»ÒªÓÃ`//`
* else ²»»»ÐÐ, ¼ÇµÃÔÚ¸ñÊ½»¯ÉèÖÃÀïÃæµ÷ºÃ¡£
## 3. ½Ó¿ÚËµÃ÷
=======
# DNS-Relay â€”â€?DNSä¸­ç»§å™?
## 1. æ¡†æž¶
![æµç¨‹å›¾](./images/design.png)
## 2. ç¼–ç è§„èŒƒ
* å˜é‡å? `yourFather`
* å‡½æ•°å? `YourFather`
* æ–‡ä»¶å? `your_father`
* ä¸è¦åœ¨`.h`æ–‡ä»¶é‡Œé¢å£°æ˜Žä»»ä½•å˜é‡
* ç»“æž„ä½“å£°æ˜Žæ”¾åœ¨`.h`ä¸?
* æ‰€æœ‰çš„å…¨å±€å˜é‡éƒ½ç”¨`static`ä¿®é¥°,éœ€è¦è®¿é—®çš„è¯æä¾›æŽ¥å?
* ä¸éœ€è¦è¢«å¤–éƒ¨è°ƒç”¨çš„å‡½æ•°ä¸ç”¨åœ¨`.h`ä¸­å£°æ˜?åªéœ€è¦åœ¨`.cpp`ä¸­ç”¨`static`ä¿®é¥°å®šä¹‰
* æ‰€æœ‰çš„å¤–éƒ¨å‡½æ•°(è¢«å…¶ä»–æ¨¡å—çš„è°ƒç”¨çš„å‡½æ•?å¿…é¡»åŠ `extern`,(è™½ç„¶è¿™ä¸ª`extern`æ²¡æœ‰ä»€ä¹ˆæ„ä¹?;
* æ‰€æœ‰è¡¨ç¤ºä¸€ä¸ªå­—èŠ‚çš„å†…å­˜åŒºå¿…é¡»ä½¿ç”¨`unsigned char`
* `include`å…¨éƒ¨æ”¾åœ¨`.h`æ–‡ä»¶å?é˜²æ­¢é‡å¤å®šä¹‰
* æ³¨é‡Šç»Ÿä¸€ä½¿ç”¨ `/* ... */` ä¸è¦ç”¨`//`
* else ä¸æ¢è¡? è®°å¾—åœ¨æ ¼å¼åŒ–è®¾ç½®é‡Œé¢è°ƒå¥½ã€?
## 3. æŽ¥å£è¯´æ˜Ž
>>>>>>> 977e2d94c133cc845dfa304a76e019622d61648a
