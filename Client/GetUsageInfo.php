<?php
$address = gethostbyaddr("127.0.0.1");
$port = 6666;
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

$result = socket_connect($socket, $address, $port);
if($result === true)
{
	$data = socket_read($socket, 2024);
	$data = explode(";", $data);
	
	if(count($data) == 7)
	{
		$arrayThatWillBeConvertIntoJSON = array('Total RAM' => $data[0],
																	'Available RAM' => $data[1],
																	'RAM Usage' => $data[2],
																	'Total Virtual Memory' => $data[3],
																	'Available Virtual Memory' => $data[4],
																	'Virtual Memory Usage' => $data[5],
																	'CPU Usage' => substr($data[6], 0, (strlen($data[6]) - 1)));
		echo json_encode($arrayThatWillBeConvertIntoJSON);
	}
	else
	{
		echo "Wrong data";
	}
}
else echo "Connect Error";
?>