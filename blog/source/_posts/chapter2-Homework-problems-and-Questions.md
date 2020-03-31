---
title: 'chapter2 :Homework problems and Questions'
date: 2020-03-31 06:23:55
categories:
- Computer Network A Top-Down Approach
tags:
- Computer Network A Top-Down Approach
---
**1.列出5个(非专有)的互联网应用，并说明他们使用的应用层协议**
- 1. Chrome (谷歌浏览器貌似不是开源的．．．,不管了用的最多,下面答案来自stackoverflow)<br>
	in term of what protocol you can use in the chrome browser bar you can use: HTTP,HTTPS,FILE and FTP.SSH is not implemented by chrome,but rather it implements SSL, it also does nit implement SMTP,but rather when you visiting a website(via HTTP or hopefully HTTPS).That website is not at all connecting to an SMTP server to display you you emails, but is merely serving up web pages and conneting to API's to display/edit/compose your email (by then which the email client's backend is connected to their SMTP server).<br>
Also chrome does implement FTP,like you can visit an IP address that has FTP enable such as : ftp://123.34.45.890 and you can use the directory listings as a webpage. An example of this would the CentOS mirrors [here](https://www.centos.org/download/mirrors/).on the right column they have FTP sites. You can access the FIP director via web browser that supports FTP or you can fire up a terminal and do ftp ftp://ftp.is.co.za/mirror/centos/.<br>
- 



