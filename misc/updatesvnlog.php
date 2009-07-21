<?php
//This is an example script for updating your 'recently changed' menu entry.
$filen = "/home/tremulous/korx/ui/help.txt";
$dir = "/home/tremulous/korx/";

exec( "echo \"`svn log $dir -l5 -v`\"", $out );
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
      if( $out[$j] != NULL )
        fwrite( $h, "    \"".$out[$j]."\"\n" );
    }
  }
}

fclose( $h );

?>
