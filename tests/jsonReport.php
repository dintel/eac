#!/usr/bin/php
<?php

$csv = file_get_contents('result.csv');
$rows = explode("\n",$csv);
unset($rows[0]);
$data = array();
foreach($rows as $row) {
    $tmp = explode(";",$row);
    if(isset($tmp[3])) {
        if($tmp[3] == 'lz77') {
            $data[$tmp[0]][$tmp[3]][] = [intval($tmp[2]), floatval($tmp[9])];
        } elseif($tmp[3] == 'eac') {
            $data[$tmp[0]][$tmp[3]][] = [intval($tmp[1]), floatval($tmp[9])];
        }
    }
}

file_put_contents('../viewer/result.json',"var data=".json_encode($data));
