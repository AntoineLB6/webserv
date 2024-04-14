#!/usr/bin/php-cgi

<html>
<body>
<h1>Form Data</h1>
<h1><?php if(isset($_GET['fname']) && isset($_GET['lname'])): ?>
        <br/>
        Your first name is <?php echo $_GET["fname"]; ?>
        <br/>
        Your last name is <?php echo $_GET["lname"]; ?>
<?php endif; ?></h1>

</body>
</html>
