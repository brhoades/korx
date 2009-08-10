<?php
//This is an example script for updating your 'recently changed' menu entry.
$filen = "/home/tremulous/korx/ui/help.txt";
$dir = "/home/tremulous/korx/";
//$filen = "/home/aaron/work/korx/ui/help.txt";
//$dir = "/home/aaron/work/korx";

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
      if( $j > 0 
          && stripos( $out[$j-1], "------------------------------------------------" ) !== FALSE )
      {
        $out[$j] = explode( " | ", $out[$j] );
        $rev = $out[$j][0];
        $name = $out[$j][1];
        $date = $out[$j][2];
        $date = explode( " (", $date );
        $date = $date[0];
        $date = explode( " ", $date );
        if( $date[2] == "-300" )
          $date[2] = "ADT";
        else if( $date[2] == "-400" )
          $date[2] = "AST/EDT";
        else if( $date[2] == "-0500" )
          $date[2] = "EST/CDT";
        else if( $date[2] == "-0600" )
          $date[2] = "CST/MDT";
        else if( $date[2] == "-700" )
          $date[2] = "MST/PDT";
        else if( $date[2] == "-800" )
          $date[2] = "PST";
        $date = implode( " ", $date );
        unset( $out[$j] );
        $out[$j] = $rev." by ".$name." on ".$date.":";
      }
      
      if( $out[$j] != NULL && stripos( $out[$j], "------------------------------------------------" ) === FALSE )
        fwrite( $h, "    \"".addcslashes( mysql_escape_string( $out[$j] ), "\$" )."\\n\"\n" );
      else if( stripos( $out[$j], "------------------------------------------------" ) !== FALSE )
        fwrite( $h, "    \"\\n\"\n" );
    }
  }
}

fclose( $h );

?>
