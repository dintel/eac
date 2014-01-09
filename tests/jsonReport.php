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
            $data[$tmp[0]][$tmp[3]]['data'][] = [intval($tmp[2]), floatval($tmp[9])];
            $data[$tmp[0]][$tmp[3]]['longest_match'][] = [intval($tmp[2]), floatval($tmp[10])];
        } elseif($tmp[3] == 'eac') {
            $data[$tmp[0]][$tmp[3]]['data'][] = [intval($tmp[1]), floatval($tmp[9])];
            $data[$tmp[0]][$tmp[3]]['longest_match'][] = [intval($tmp[1]), floatval($tmp[10])];
            $data[$tmp[0]][$tmp[3]]['avg_longest_match'][] = [intval($tmp[1]), floatval($tmp[11])];
            $data[$tmp[0]][$tmp[3]]['deviation'][] = [intval($tmp[1]), floatval($tmp[12])];
        }
    }
}

file_put_contents('../viewer/result.json',"var data=".json_encode($data));
