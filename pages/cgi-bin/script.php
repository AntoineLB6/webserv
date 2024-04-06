<html>
<body>

<?php if(isset($_GET['fname']) && isset($_GET['lname'])): ?>
        <br/>
        Your first name is <?php echo $_GET["fname"]; ?>
        <br/>
        Your last name is <?php echo $_GET["lname"]; ?>
<?php endif; ?>


</body>
</html>
