<?php
//This is an example script for updating your 'recently changed' menu entry.
$filen = "/home/tremulous/korx/ui/help.txt";
$dir = "/home/tremulous/korx/";

exec( "echo \"`svn log $dir -l4`\"", $out );
exec( "svn revert $filen" );

$endstring = "korx.googlecode.com.";
$h = fopen( $filen, "r" ) or die( "Error" );
$bytes = 0;
$i=0;
while( !feof( $h ) )
{
  $file[$i] = fgets( $h );
  $i++;
}

fclose( $h );
$h = fopen( $filen, "w" );

for( $i=0; $i<count( $file ); $i++ )
{
  fwrite( $h, $file[$i] );
  if( stripos( $file[$i], $endstring ) !== FALSE )
  {
    for( $j=0; $j<count( $out ); $j++ )
    {
      if( stripos( $out[$j], "lines" ) !== FALSE && stripos( $out[$j], " | " !== FALSE )
      {
        $out[$j] = explode( " | ", $out[$j] );
        $rev = $out[$j][0];
        $name = $out[$j][1];
        $date = $out[$j][2];
        $date = explode( " (", $date );
        $date = $date[0];
        $out[$j] = $rev". by ".$name." on ".$date.":";
      }
      
      if( $out[$j] != NULL && stripos( $out[$j], "------------------------------------------------" ) === FALSE )
        fwrite( $h, "    \"".$out[$j]."\\n\"\n" );
      else if( stripos( $out[$j], "------------------------------------------------" ) !== FALSE )
        fwrite( $h, "    \"\\n\"\n" );
    }
  }
}

fclose( $h );

?>
