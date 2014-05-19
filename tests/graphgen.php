#!/usr/bin/php
<?php

function getGnuPlotProgram($title,$outputFilename,$xLabel,$yLabel,$firstTitle = "LZ77",$secondTitle = "EAC") {
    // return "set terminal jpeg size 1024,768 font \"Gill Sans, 9\" rounded\n" .
    return "set terminal pngcairo size 1024,768 enhanced font \"Gill Sans, 9\" \n" .
    "# define axis\n" .
    "# remove border on top and right and set color to gray\n" .
    "set style line 11 lc rgb '#808080' lt 1\n" .
    "set border 3 back ls 11\n" .
    "set tics nomirror\n" .
    "# define grid\n" .
    "set style line 12 lc rgb '#808080' lt 0 lw 1\n" .
    "set grid back ls 12\n" .
    "# color definitions\n" .
    "set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red\n" .
    "set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green\n" .
    "set key bottom right\n" .
    "set xlabel '{$xLabel}'\n" .
    "set ylabel '{$yLabel}'\n" .
    "set logscale x 2\n" .
    "set title \"{$title}\" textcolor rgb \"#6a6a6a\" font \"SVBasic Manual, 12\"\n" .
    "set output '$outputFilename'\n" .
    "plot '-' title \"{$firstTitle}\" w lp ls 1, '-' using 1:2:($2) with labels offset 0,-1 notitle, '-' title \"{$secondTitle}\"  w lp ls 2, '-' using 1:2:($2) with labels offset 0,1 notitle\n";
}

const FILENAME = 0;
const BLOCK_SIZE = 1;
const WINDOW_SIZE = 2;
const ALGORITHM = 3;
const ENCODE_TIME = 4;
const DECODE_TIME = 5;
const RESULT = 6;
const FILE_SIZE = 7;
const COMPRESSED_SIZE = 8;
const RATIO = 9;
const FILE_LONGEST_MATCH = 10;
const AVERAGE_LONGEST_MATCH = 11;
const STD_DEVIATION_OF_LONGEST_MATCH = 12;
const BLOCK_LONGEST_MATCH = 13;

const NUM_OF_FIELDS_IN_LINE = 14;

const IMAGES_DIRECTORY = "images";

const SERIES_DELIMETER = "e\n";

const OUTPUT_EXTENSION = "png";

if($argc != 2) {
    echo "Usage: {$argv[0]} <result.csv>\n";
    exit(1);
}

$tmp = file_get_contents($argv[1]);
if($tmp === false) {
    echo "Could not read file {$argv[1]}, exiting.\n";
    exit(2);
}

$data = [];
$lines = explode("\n",$tmp);
unset($lines[0]);
foreach($lines as $line) {
    $fields = explode(";",$line);
    if(count($fields) != NUM_OF_FIELDS_IN_LINE)
        continue;
    $x = $fields[ALGORITHM] === "lz77" ? WINDOW_SIZE : BLOCK_SIZE;
    $data[$fields[FILENAME]]["Compression ratio"][$fields[ALGORITHM]][] = ['x' => $fields[$x], 'y' => $fields[RATIO]];
    $data[$fields[FILENAME]]["Longest match"][$fields[ALGORITHM]][] = ['x' => $fields[$x], 'y' => $fields[FILE_LONGEST_MATCH]];
    if($fields[ALGORITHM] == "eac") {
        $data[$fields[FILENAME]]["Average longest match and deviation"]['match'][] = ['x' => $fields[$x], 'y' => $fields[AVERAGE_LONGEST_MATCH]];
        $data[$fields[FILENAME]]["Average longest match and deviation"]['deviation'][] = ['x' => $fields[$x], 'y' => $fields[STD_DEVIATION_OF_LONGEST_MATCH]];
    }
}


foreach($data as $filename => $multiSeries) {
    foreach($multiSeries as $title => $series) {
        $outputFilename = IMAGES_DIRECTORY . "/" . str_replace('.','_',$filename . "-" . strtolower(str_replace(" ","-",$title))) . "." . OUTPUT_EXTENSION;
        if($title == "Compression ratio") {
            $yLabel = "Compression ratio";
        }
        if($title == "Longest match") {
            $yLabel = "Longest match length (bits)";
        }
        if($title == "Average longest match and deviation") {
            $yLabel = "Bits";
        }
        $gnuplot = popen("gnuplot","w");
        if($title == "Average longest match and deviation") {
            fwrite($gnuplot,getGnuPlotProgram($title . " ($filename)",$outputFilename,"Window size / Block size",$yLabel,"Avg. longest match","Std. deviation"));
            foreach($series['match'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['match'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['deviation'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['deviation'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
        } else {
            fwrite($gnuplot,getGnuPlotProgram($title . " ($filename)",$outputFilename,"Window size / Block size",$yLabel));
            foreach($series['lz77'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['lz77'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['eac'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
            foreach($series['eac'] as $point) {
                fwrite($gnuplot,"{$point['x']}.0 {$point['y']}\n");
            }
            fwrite($gnuplot,SERIES_DELIMETER);
        }
        pclose($gnuplot);
    }
}

echo "Done!\n";
