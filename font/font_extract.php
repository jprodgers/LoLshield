<?php


$d=opendir(".");
$vstar=array();
$vmin=128;
$vmax=0;

while (($c=readdir($d))!==false) {
  if (substr($c,0,1)!="." && strlen($c)==1) {
    $letters[$c]=explode("\n",file_get_contents($c));
    
    $ct=0;
    echo "uint8_t letters_".ord($c)."[] = { ";
    $o=ord($c);
    if ($o<$vmin) $vmin=$o;
    if ($o>$vmax) $vmax=$o;
    $vstar[$o] = 1; 
    for($j=0;$j<=5;$j++) {
      for($k=0;$k<=8;$k++) {
	if (substr($letters[$c][$k],$j,1)=="O") {
	  echo "".$j.",".$k.", ";
	  $ct++;
	}
      }
    }
      echo "9,9"; 
    echo " };\n";
  }
 } // while
closedir($d);

echo "\n";

$already=false;
echo "void * font[] = { ";
for($i=$vmin;$i<=$vmax;$i++) {
  if ($already) echo ", ";
  if ($vstar[$i]) {
    echo " &letters_".$i." /*".chr($i)."*/";
  } else {
    echo " 0";
  }
  $already=true;
 }
echo "\n};\n\n";

echo "uint8_t fontMin=$vmin;\n";
echo "uint8_t fontMax=$vmax;\n";

