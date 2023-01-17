
<?php
require $_SERVER['DOCUMENT_ROOT'].'/CITRUS/config/sqlConfig.php';

//change these if needed. is in litre/second.
$flowrate_w = 0.225;
$flowrate_c = 0.225;

//limitConfig[x][y], where x is CNC, y is value from DB table
//DB table(y) - CNC, tankCapacity, coolantLvMax, concentration, coolantLv, concentrationLimit, coolantLvLimit

function updateConfig(){
	global $servername;
	global $username;
	global $password;
	global $database;
	$mysqli = mysqli_connect($servername, $username, $password, $database);
	if (mysqli_connect_errno()){echo "?".mysqli_connect_error(); exit();}

	$query = "SELECT * FROM zappa;";
	$result = mysqli_query($mysqli, $query);
	$row1 = mysqli_fetch_row($result);
	mysqli_next_result($mysqli);
	$row2 = mysqli_fetch_row($result);

	mysqli_close($mysqli);
	return array($row1, $row2);
	//returns $limitConfig basically
}

function updateSQL($limitConfig){
	global $servername;
	global $username;
	global $password;
	global $database;
	$mysqli = mysqli_connect($servername, $username, $password, $database);
	if (mysqli_connect_errno()){echo mysqli_connect_error(); exit();}

	for($i = 0; $i < 2; $i++){ //this janky code is because I can't put a double array in quotations
		$n1 = $limitConfig[$i][0];
		$n2 = $limitConfig[$i][1];
		$n3 = $limitConfig[$i][2];
		$n4 = $limitConfig[$i][3];
		$n5 = $limitConfig[$i][4];
		$n6 = $limitConfig[$i][5];
		$n7 = $limitConfig[$i][6];
		
		$query = "INSERT INTO zappa VALUES (
		    $n1,
			$n2,
			$n3,
			$n4,
			$n5,
			$n6,
			$n7
			)
			ON DUPLICATE KEY UPDATE 
			tankCapacity=VALUES(tankCapacity), 
			coolantLvMax=VALUES(coolantLvMax), 
			concentration=VALUES(concentration), 
			coolantLv=VALUES(coolantLv),
			concentrationLimit=VALUES(concentrationLimit), 
			coolantLvLimit=VALUES(coolantLvLimit);";

		mysqli_query($mysqli, $query);	
	}
	mysqli_close($mysqli);
}

function calculations($limitConfig, $cnc){ //prevention of calculating funny values done via main.php					
	global $flowrate_w;
	global $flowrate_c;

	global $volume;	
	global $coolantLitres;
	global $waterLitres;

	$volume	= $limitConfig[$cnc][4]/100 * $limitConfig[$cnc][1]; 
	$coolantLitres = $limitConfig[$cnc][3]/100 * $volume ;  
	$waterLitres	= (1 - $limitConfig[$cnc][3]/100) * $volume;
	$optimalVolume = $coolantLitres / ($limitConfig[$cnc][5]/100); 

	$runtimeWater = $optimalVolume - $volume; 
	$runtimeWater = $runtimeWater / $flowrate_w *1000;

	$runtimeCoolant = ($limitConfig[$cnc][6]/100* $limitConfig[$cnc][1]);
    	$runtimeCoolant = ($runtimeCoolant - $volume) / $flowrate_c *1000; 

	return array(round($runtimeWater), round($runtimeCoolant));
}

function runFinish($runtimeConfig, $cnc, $duration, $limitConfig){ 
	//rtcfg is 0 or 1, 0 water 1 coolant
	global $flowrate_w;
	global $flowrate_c;
	$flowrates = array($flowrate_w, $flowrate_c);
	global $volume;	
	global $coolantLitres;
	global $waterLitres;
	
	calculations($limitConfig, $cnc);

	$update_conc = $limitConfig[$cnc][3];
	
	$pumped = ($duration * $flowrates[$cnc] / 1000); 
	$update_cool = ($pumped + $volume ) / ($limitConfig[$cnc][1]/100);

   	if ($runtimeConfig == 0){
    	$update_conc = ($coolantLitres / ($pumped +$waterLitres + $coolantLitres))*100;
	}

	$limitConfig[$cnc][3] = $update_conc;
	$limitConfig[$cnc][4] = $update_cool;
	updateSQL($limitConfig);

	global $runtimes;
	$runtimes[$cnc] = calculations($limitConfig, $cnc);
	return $limitConfig;
}

?>
