<?php
//this chunk of php code checks for GET requests from the arduino, then outputs runtime at the top of the page for the arduino to read
//else, the page displays normally for users
require './functions/sqlFunc.php';
$limitConfig= updateConfig(); 
//limitConfig[x][y], where x is CNC, y is value from DB table
//DB table(y) - CNC, tankCapacity, coolantLvMax, concentration, coolantLv, concentrationLimit, coolantLvLimit

$query = "SELECT * from zappa";
$db = mysqli_connect($servername, $username, $password, $database);
$results = mysqli_query($db,$query);

if ($_SERVER["REQUEST_METHOD"] == "GET") {
	if(isset($_GET['type'])){
	        $type = $_GET['type'];
	        if ($type == "download"){
	                $cnc= $_GET['cnc']; 
	                $cfg = $_GET['cfg'];    
	                $override = $_GET['override'];
	
	                if ($override == 1){
	                        $limitConfig[$cnc][6] = $limitConfig[$cnc][2];
	                        echo "override triggered";
	                }
	
	                if ($cfg == 0 && ($limitConfig[$cnc][3] <= $limitConfig[$cnc][5])){
	                        echo "failure(water), ". $limitConfig[$cnc][3]." less than or equal to ".$limitConfig[$cnc][5];
	                        echo " #"."0"."<br>"; //this format IS IMPORTANT. the arduino expects and depends on this format to read runtime
	                }
	
	                elseif ($limitConfig[$cnc][6] <= $limitConfig[$cnc][4]){ 
	                        echo "failure(coolant), ". $limitConfig[$cnc][6]." less than or equal to ".$limitConfig[$cnc][4];
	                        echo " #"."0"."<br>";
			}

	                else{
	                        $runtimes[$cnc] = calculations($limitConfig, $cnc);
	                        echo " #".$runtimes[$cnc][$cfg]."<br>";
	                }
	        }
	
	        if ($type == "upload"){ 
	                $duration = $_GET['duration']; 
	                $runtimeConfig = $_GET['runtimeConfig'];
	                $cnc = $_GET['cnc'];
	                runFinish($runtimeConfig, $cnc, $duration, $limitConfig);
		}
	}
}


?>

<!DOCTYPE html>
<html>
<head>
	<title>Coolant Calculator</title>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.0/jquery.min.js"></script>
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" />  
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>            
  <script src="jquery.tabledit.min.js"></script>
  <script src="jquery-editable-select.min.js"></script>

<style>
.myTable { 
  width: 100%;
  text-align: center;
  background-color: lemonchiffon;
  border-collapse: collapse; 
  }
.myTable th { 
  background-color: goldenrod;
  color: white; 
  }
.myTable td, 
.myTable th { 
  padding: 10px;
  border: 1px solid goldenrod; 
  }
</style>
</head>
<center><h1><em>CNC Machine Monitoring<em></h1></center>
<body>
<center>
<form action="" method="post">
  
    <p>
    <div class="container"> 
      <div class="table-responsive">

    <table id="editable_table" class="table table-voedered table-striped">
	<tr>
    <th><center>CNC<center></th>
		<th><center>Tank Capacity<center></th>
		<th><center>Coolant Max LVL<center></th>
    <th><center>concentration<center></th>
    <th><center>coolant level<center></th>
    <th><center>concentrationLimit<center></th>
    <th><center>Coolant Limit Level<center></th>
    <th><center>EditPage<center></th>
    
    
	<tr>


 
 
  <td><center><?php echo $limitConfig[0][0]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][1]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][2]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][3]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][4]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][5]; ?> <center></td>
  <td><center><?php echo $limitConfig[0][6]; ?> <center></td>
  <td><a href="./functions/login.php" class="btn btn-primary">Editpage</a></td>
  

  
  

  <tr>

  <td><center><?php echo $limitConfig[1][0]; ?> <center></td>
  <td><center><?php echo $limitConfig[1][1]; ?> <center></td>
  <td><center><?php echo $limitConfig[1][2]; ?> <center></td>
  <td><center><?php echo $limitConfig[1][3]; ?> <center></td>
  <td><center><?php echo $limitConfig[1][4]; ?><center></td>
  <td><center><?php echo $limitConfig[1][5]; ?> <center></td>
  <td><center><?php echo $limitConfig[1][6]; ?> <center></td>
  <td><a href="./functions/login.php"class="btn btn-primary">Editpage</a></td>


    <?php
    echo 'How to use: This is a edit page. This is only for you to see the values. Press "EDITPAGE" to change the values.'

    ?>
  
  

  
  </tr>
</form>

</body>
</html>







