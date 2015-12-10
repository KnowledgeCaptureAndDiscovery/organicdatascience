<?php

global $SCHEMA;

$SCHEMA["LiPD"] = array(
	"_id" => array("{dataSetName}"),

	"dataSetName" => array( 
		"name"=>"Dataset Name",
		"type"=>"Text",
		"standard"=>true
	),
	"archiveType" => array( 
		"type"=>"Page", 
		"standard"=>true,
		"icon"=>"fa-archive" 
	),
	"collectionName" => array( 
		"type"=>"Page",
		"standard"=>true
	),
	"investigators" => array( 
		"type"=>"Page", 
		"category"=>"Person", 
		"schema"=>"Author", 
		"multiple"=>true, 
		"standard"=>true,
		"fromJson"=>"parseAuthorString",
		"icon"=>"fa-user"
	),
	"funding" => array( 
		"type"=>"Page", 
		"category"=>"Funding", 
		"schema"=>"Funding", 
		//"standard"=>true,
		"icon"=>"fa-dollar" 
	),
	"pub" => array( 
		"name"=>"Publication", 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"Publication", 
		"schema"=>"Publication",
		"standard"=>true,
		"icon"=>"fa-leanpub"
	),
	"geo" => array( 
		"name"=>"GeoLocation", 
		"type"=>"Page", 
		"category"=>"GeoLocation", 
		"schema"=>"GeoLocation", 
		"fromJson"=>"parseGeoLocation",
		"toJson"=>"locationToJson",
		"standard"=>true,
		"icon"=>"fa-location-arrow"
	),
	"paleoData" => array( 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"UserProvidedData", 
		"schema"=>"LiPDData",
		"standard"=>true,
		"icon"=>"fa-globe"
	),
	"chronData" => array( 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"UserProvidedData", 
		"schema"=>"LiPDData",
		"standard"=>true,
		"icon"=>"fa-clock-o" 
	),
	"googleSpreadSheetKey" => array( 
		"name"=>"Google_Spreadsheet", 
		"type"=>"URL", 
		"fromJson"=>"getGoogleSpreadsheetURL",
		"toJson"=>"getGoogleSpreadsheetKey",
		"icon"=>"fa-table"
	)
);

$SCHEMA["Funding"] = array(
	"_id" => array( "{fundingAgency}", "_" , "{fundingGrant}" ),

	"fundingAgency" => array ( 
		"type"=>"Page",
		"standard"=>true
	),
	"fundingGrant" => array ( 
		"type"=>"Page",
		"standard"=>true
	)
);

$SCHEMA["Publication"] = array(
	"_id" => array("Publication.", "{identifier.0.id|doi|_parent.dataSetName}"),

	"pubYear" => array( 
		"name"=>"Publication_year",
		"standard"=>true,
		"type"=>"Number" 
	),
	"type" => array( 
		"name"=>"Publication_type",
		"standard"=>true,
		"icon"=>"newspaper-o"
	),

	// The following added for dealing with Single line authors
	"authors" => array( 
		"name"=>"Author", 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"Person", 
		"schema"=>"Author", 
		"fromJson"=>"parseAuthorString",
		"standard"=>true,
		"hack"=>true,
		"icon"=>"fa-user"
	),
	"author" => array( 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"Person", 
		"schema"=>"Author", 
		"fromJson"=>"parseAuthors",
		"icon"=>"fa-user" 
	),
	"identifier" => array( 
		"name"=>"Publication_Identifier", 
		"type"=>"Page", 
		"category"=>"Publication_Identifier", 
		"schema"=>"Publication_Identifier", 
		"standard"=>true,
		"multiple"=>true 
	)
);

$SCHEMA["Publication_Identifier"] = array(
	"_id" => array("{id}"),

	"type" => array(
		"name"=>"Identifier_type",
		"standard"=>true
	),
	"id" => array(
		"name"=>"Identifier",
		"standard"=>true
	),
	"url" => array(
		"name"=>"IdentifierURL", 
		"type"=>"URL",
		"standard"=>true,
		"icon"=>"fa-link"
	)
);

$SCHEMA["LiPDData"] = array(
	"_id" => array("{filename}", "_trunc(4)"),

	"filename" => array( 
		"name"=>"DataWikiLocation", 
		"type"=>"File" 
	),
	"columns" => array( 
		"type"=>"Page", 
		"multiple"=>true, 
		"category"=>"DataType", 
		"schema"=>"LiPDColumn",
		"icon"=>"fa-columns" 
	)
);

$SCHEMA["LiPDColumn"] = array(
	"_id" => array("{TSid}"),

	"number" => array("name"=>"Index", "type"=>"Number", "subobject"=>true),
	"parameter" => array(),
	"units" => array(),
	"dataType" => array(),
	"description" => array(),

	"calibration" => array(
		"type"=>"Page", 
		"category"=>"Calibration"
	),
	"climateInterpretation" => array(
		"type"=>"Page", 
		"category"=>"Climate_Interpretation"
	)
);

$SCHEMA["GeoLocation"] = array(
	"_id" => array("{_parent.dataSetName}", ".Location"),

	"siteName" => array( "type"=>"Page", "standard"=>true ),
	"coordinates" => array( "type"=>"Geographic_coordinate", "standard"=>true, "icon"=>"fa-location-arrow"),
	"coordinatesFor" => array( "type"=>"Page", "standard"=>true ),
	"altitude" => array( "type"=>"Number", "standard"=>true )
);

$SCHEMA["Author"] = array(
	"_id" => array("{name}")
);

function parseAuthorString($authstring, $parentid=null) {
	$authors = array();
	if(preg_match("/\s*;\s*/", $authstring)) {
		$auths = preg_split("/\s*;\s*/", $authstring);
		foreach($auths as $auth) {
			if(preg_match("/(.+)\s*,\s*(.+)/", $auth, $m)) 
				$authors[] = array("name"=> $m[2]." ".$m[1]);
			else 
				$authors[] = array("name"=> $auth);
		}
	}
	else if(preg_match("/.*,.*,.*/", $authstring)) {
		$auths = preg_split("/\s*,\s*/", $authstring);
		for($i=0; $i<sizeof($auths); $i++) {
			$name = $auths[$i];
			if(!preg_match("/\s/", $name)) {
				$i++;
				$name = $auths[$i]." ".$name;
			}
			$authors[] = array("name"=>$name);
		}
	}
	else if(preg_match("/(.+),(.+)/", $authstring, $m)) {
		$authors[] = array("name"=>$m[2]." ".$m[1]);
	}
	else {
		$authors[] = array("name"=>$authstring);
	}
	return $authors;
}

function parseAuthors($auths, $parentid=null) {
	$authors = array();
	if(!is_array($auths))
		return null;

	foreach($auths as $auth) {
		if(preg_match("/(.+)\s*,\s*(.+)/", $auth["name"], $m)) 
			$authors[] = array("name" => $m[2]." ".$m[1]);
		else 
			$authors[] = array("name" => $auth["name"]);
	}
	return $authors;
}

function parseGeoLocation($geo, $parentid=null) {
	$ngeo = array();
	$ngeo["locationType"] = $geo["type"];
	$ngeo["coordinatesFor"] = $parentid;
	$ngeo["coordinates"] = $geo["geometry"]["coordinates"][0].",".$geo["geometry"]["coordinates"][1];
	if(sizeof($geo["geometry"]["coordinates"]) > 2)
		$ngeo["altitude"] = $geo["geometry"]["coordinates"][2];
	foreach($geo["properties"] as $key=>$value) {
		$ngeo[$key] = $value;
	}
	return $ngeo;
}

function locationToJson($geo, $parentid=null) {
	$geojson = array("geometry"=>array("coordinates"=>array()), "properties"=>array());
	foreach($geo as $prop=>$value) {
		if($prop == "locationType") {
			$geojson["type"] = $geo["locationType"];
		}
		else if($prop == "coordinates") {
			$latlong = explode(",", $geo["coordinates"]);
			$geojson["geometry"]["coordinates"][0] = $latlong[0];
			$geojson["geometry"]["coordinates"][1] = $latlong[1];
		}
		else if($prop == "altitude") {
			$geojson["geometry"]["coordinates"][2] = $latlong[2];
		}
		else if($prop == "coordinatesFor") {
			// Ignore
		}
		else {
			$geojson["properties"][$prop] = $value;
		}
	}
	return $geojson;
}

function getGoogleSpreadsheetURL($key, $parentid=null) {
	return "https://docs.google.com/spreadsheets/d/$key";
}

function getGoogleSpreadsheetKey($url, $parentid=null) {
	return str_replace("https://docs.google.com/spreadsheets/d/", "", $url);
}

?>
