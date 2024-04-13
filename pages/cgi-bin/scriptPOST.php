#!/usr/bin/php-cgi

<html>
<body>
<h1><?php if(isset($_POST['fname']) && isset($_POST['lname'])): ?>
        <br/>
        <h1>Your first name is <?php echo $_POST["fname"]; ?>
        <br/>
        Your last name is <?php echo $_POST["lname"]; ?>
<?php endif; ?></h1>


</body>
</html>
