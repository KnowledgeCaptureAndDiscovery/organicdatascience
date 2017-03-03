<?php

global $SCHEMA;

$SCHEMA["Dataset"] = array(
	"@id" => array("{dataSetName}"),

	"dataSetName" => array( 
		"name"=>"Name" , 
		"alternates"=>["paleoArchiveName"] 
	),
	"originalDataURL" => array( 
		"name"=>"HasLink" , 
		"alternates"=>["dataURL"] 
	),
	"dataContributor" => array( 
		"name"=>"Author", 
		"schema"=>"Person", 
		"alternates"=>["whoEnteredinDB", "MetadataEnteredByWhom"],
		"fromJson"=>"parsePerson"
	),
	"archiveType" => array( 
		"name"=>"archiveType", 
		"alternates"=>["paleoDataArchive", "paleoData_Archive"] 
	),
	"investigator" => array( 
		"name"=>"Contributor" , 
		"schema"=>"Person", 
		"multiple"=>true,
		"fromJson"=>"parsePersons",
	),
	// The following added for dealing with Single line investigators
	"investigators" => array( 
		"name"=>"Contributor" , 
		"schema"=>"Person", 
		"hack"=>true,
		"fromJson"=>"parsePersonsString",
	),
	"funding" => array( 
		"name"=>"FundedBy", 
		"multiple"=>true,
		"schema"=>"Funding"
	),
	"pub" => array( 
		"name"=>"PublishedIn", 
		"multiple"=>true,
		"schema"=>"Publication"
	),
	"geo" => array( 
		"name"=>"CollectedFrom", 
		"schema"=>"Location", 
		"fromJson"=>"parseLocation",
		"toJson"=>"locationToJson"
	),
	"paleoData" => array( 
		"name"=>"IncludesPaleoData", 
		"multiple"=>true,
		"schema"=>"PaleoData"
	),
	"chronData" => array( 
		"name"=>"IncludesChronData", 
		"multiple"=>true,
		"schema"=>"ChronData"
	),
	"googleSpreadSheetKey" => array( 
		"name"=>"HasSpreadsheetLink", 
		"fromJson"=>"getGoogleSpreadsheetURL",
		"toJson"=>"getGoogleSpreadsheetKey"
	),
	"dataSetVersion" => array("name"=>"DatasetVersion")
);

$SCHEMA["Funding"] = array(
	"@id" => array( "{fundingAgency|agency}", ".", "{fundingGrant|grant}" ),

	"agency" => array ( 
		"name"=>"FundingAgency",
		"alternates"=>["fundingAgency"]
	),
	"grant" => array ( 
		"name"=>"GrantNumber",
		"multiple"=>true,
		"alternates"=>["fundingGrant"]
	),
	"country" => array ( 
		"name"=>"FundingCountry",
		"alternates"=>["fundingCountry"]
	)
);

$SCHEMA["Publication"] = array(
	"@id" => array("Publication.", "{identifier.0.id|doi|@parent.dataSetName}"),
	"@fromJson"=> array("setIdentifierProperties"),
	"@toJson" => array("createPublicationIdentifier"),

	"title" => array( 
		"name"=>"Title"
	),
	"pubYear" => array( 
		"name"=>"PublicationYear",
		"alternates"=>["year"]
	),
	"citation" => array( 
		"name"=>"Citation",
		"alternates"=>["reference"]
	),
	"link" => array( 
		"name"=>"HasLink",
		"multiple"=>true
	),
	"author" => array( 
		"name"=>"Author",
		"schema"=>"Person",
		"multiple"=>true,
		"fromJson"=>"parsePersons"
	),

	// The following added for dealing with Single line authors
	"authors" => array( 
		"name"=>"Author", 
		"schema"=>"Person", 
		"fromJson"=>"parsePersonsString",
		"hack"=>true
	)
);

$SCHEMA["PaleoData"] = array(
	"@id" => array("{@parent.dataSetName}", ".PaleoData", "{@index}"),
	"paleoDataName" => array("name"=>"Name"),

	"paleoMeasurementTable" => array( 
		"name"=>"FoundInMeasurementTable", 
		"multiple"=>true, 
		"schema"=>"DataTable"
	),
	"paleoModel" => array(
		"name"=>"PaleoModeledBy",
		"multiple"=>true,
		"schema"=>"Model",
		"category"=>"PaleoModel"
	)
);

$SCHEMA["ChronData"] = array(
	"@id" => array("{@parent.dataSetName}", ".ChronData", "{@index}"),

	"chronMeasurementTable" => array( 
		"name"=>"FoundInMeasurementTable", 
		"multiple"=>true,
		"schema"=>"DataTable"
	),
	"chronModel" => array(
		"name"=>"ChronModeledBy",
		"multiple"=>true,
		"schema"=>"Model",
		"category"=>"ChronModel"
	)
);

$SCHEMA["Model"] = array(
	"@id" => array("{@parent.@id}", ".Model", "{@index}"),
	"method" => array(
		"name"=>"HasCode",
		"schema"=>"SoftwareCode"
	),
	"summaryTable" => array( 
		"name"=>"FoundInSummaryTable", 
		"schema"=>"DataTable"
	),
	"ensembleTable" => array( 
		"name"=>"FoundInEnsembleTable", 
		"schema"=>"DataTable"
	),
	"distributionTable" => array( 
		"name"=>"FoundInDistributionTable", 
		"multiple"=>true, 
		"schema"=>"DataTable"
	)
);

$SCHEMA["SoftwareCode"] = array(
	"@id" => array("{@parent.@id}", ".", "{name|software}"),
	"runCommand" => array("name"=>"HasExecutionCommand"),
	"runEnv" => array("name"=>"HasExecutionEnvironment"),
	"parameters" => array("type"=>"Text"),
	"software" => array("name"=>"Name")
);

$SCHEMA["DataTable"] = array(
	"@id" => array("{filename}", "_trunc(4)"),
	"@fromJson" => array("checkForDepthVariable"),
	"@toJson" => array("removeDepthProperty"),

	"filename" => array( 
		"name"=>"hasFileName", 
		"type"=>"File" 
	),
	"columns" => array( 
		"name"=>"IncludesVariable", 
		"multiple"=>true, 
		"schema"=>"Variable"
	)
);

$SCHEMA["Variable"] = array(
	"@id" => array("{TSid|@parent.@id}", ".", "{variableName}"),
	"@fromJson" => array("setVariableCategory", "wrapUncertainty", "createProxySystem", "addFoundInTable"),
	"@toJson" => array("setVariableType", "unwrapUncertainty", "extractFromProxySystem", "removeFoundInTable"),

	"number" => array("name"=>"HasColumnNumber", "type"=>"Number"),
	"TSid" => array("name"=>"HasID"),
	"variableName" => array("name"=>"Name"),
	"units" => array("name"=>"HasUnits"),
	"measurementMethod" => array("name"=>"Method"),
	"measurementStandard" => array("name"=>"Standard"),
	"missingValue" => array("name"=>"HasMissingValue"),
	"instrument" => array(
		"name"=>"measuredBy", 
		"type"=>"Page",
		"category"=>"Instrument"
	),
	"calibration" => array(
		"name"=>"CalibratedWith", 
		"schema"=>"CalibrationModel"
	),
	"climateInterpretation" => array(
		"name"=>"InterpretedAs",
		"schema"=>"ClimateInterpretation",
		"category"=>"ClimateInterpretation"
	),
	"isotopeInterpretation" => array(
		"name"=>"InterpretedAs",
		"schema"=>"IsotopeInterpretation",
		"category"=>"IsotopeInterpretation"
	)
	/*,
	"proxySystemModel" => array(
		"name"=>"ProxySystemModel",
		"schema"=>"ProxySystemModel"
	)*/
);

$SCHEMA["ProxySystemModel"] = array(
	"@id" => array("{@parent.@id}", ".ProxySystemModel"),
	"method" => array(
		"name"=>"HasCode",
		"schema"=>"SoftwareCode"
	)
);

$SCHEMA["Location"] = array(
	"@id" => array("{@parent.dataSetName}", ".Location"),

	"siteName" => array( "name"=>"Name"),
	"coordinates" => array( "type"=>"Geographic_coordinate" ),
	"coordinatesFor" => array( "type"=>"Page" )
);

$SCHEMA["ClimateInterpretation"] = array(
	"@id" => array("{@parent.@id}", ".ClimateInterpretation"),
	"@toJson" => array("changeSeasonalityType"),

	"interpDirection" => array("name"=>"InterpretationDirection", "alternates"=>["dir", "interpDir", "direction"]),
	"variable" => array("name"=>"Name"),
	"variableDetail" => array("name"=>"Detail"),
	"rank" => array("name"=>"hasRank"),
	"basis"=> array("name"=>"RelevantQuote"),
	"local"=> array("name"=>"IsLocal")
);

$SCHEMA["IsotopeInterpretation"] = array(
	"@id" => array("{@parent.@id}", ".IsotopeInterpretation"),
	"@fromJson" => array("wrapIntegrationTime"),
	"@toJson" => array("unwrapIntegrationTime"),
	"integrationTime" => array(
		"name"=>"HasIntegrationTime",
		"type"=>"Page",
		"schema"=>"IntegrationTime"
	),
	"independentVariable" => array(
		"name"=>"HasIndependentVariable",
		"schema"=>"IndependentVariable",
		"multiple"=>true
	)
);

$SCHEMA["IntegrationTime"] = array(
	"@fromJson" => array("wrapUncertainty"),
	"@toJson" => array("unwrapUncertainty"),
	"basis"=> array("name"=>"RelevantQuote"),
	"units" => array("name"=>"HasUnits"),
	"independentVariable" => array(
		"name"=>"HasIndependentVariable",
		"schema"=>"IndependentVariable",
		"multiple"=>true
	)
);

$SCHEMA["IndependentVariable"] = array(
	"@id"=> array("{@parent.@id}", ".", "{name}"),
	"basis"=> array("name"=>"RelevantQuote"),
	"direction" => array("name"=>"InterpretationDirection", "alternates"=>["dir", "interpDir", "interpDirection"]),
	"mathematicalRelation"=> array("name"=>"Equation"),
	"rank" => array("name"=>"hasRank")
);

$SCHEMA["CalibrationModel"] = array(
	"@id" => array("{@parent.TSid}", ".Calibration"),
	"@fromJson" => array("wrapUncertainty"),
	"@toJson" => array("unwrapUncertainty"),
	"reference"=> array("name"=>"RelevantQuote"),
);

$SCHEMA["Person"] = array(
	"@id" => array("{name}")
);
$SCHEMA["Uncertainty"] = array(
	"@id" => array("{@parent.@id}", ".Uncertainty")
);

function parsePersonsString($authstring, $parent=null) {
	$authors = array();
	if(is_array($authstring))
		return parsePersons($authstring, null);

	if(preg_match("/\s*;\s*/", $authstring)) {
		$auths = preg_split("/\s*;\s*/", $authstring);
		foreach($auths as $auth) {
			$authors[] = parsePerson($auth);
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

function parsePerson($auth, $parent=null) {
	$authname = $auth;
	if(is_array($auth)) 
		$authname = $auth["name"];

	if(preg_match("/(.+)\s*,\s*(.+)/", $authname, $m)) 
		return array("name" => $m[2]." ".$m[1]);
	else 
		return array("name" => $authname);
}

function parsePersons($auths, $parent=null) {
	$authors = array();
	if(!is_array($auths))
		return null;

	foreach($auths as $auth) {
		$authors[] = parsePerson($auth, $parent);
	}
	return $authors;
}

function parseLocation($geo, $parent=null) {
	$ngeo = array();
	$ngeo["locationType"] = $geo["type"];
	$ngeo["coordinatesFor"] = $parent["@id"];
	$ngeo["coordinates"] = $geo["geometry"]["coordinates"][1].",".$geo["geometry"]["coordinates"][0];
	$ngeo["Wgs84:Lat"] = $geo["geometry"]["coordinates"][1];
	$ngeo["Wgs84:Long"] = $geo["geometry"]["coordinates"][0];
	// FIXME: For now assuming points
	$wkt = "POINT(". $geo["geometry"]["coordinates"][1]." ".$geo["geometry"]["coordinates"][0];
	if(sizeof($geo["geometry"]["coordinates"]) > 2) {
		$ngeo["Wgs84:Alt"] = $geo["geometry"]["coordinates"][2];
		$wkt .= " ".$geo["geometry"]["coordinates"][2];
	}
	$wkt .= ")";
	$ngeo["Geo:HasGeometry"] = array("@id"=>$parent["@id"].".Geometry", "@category"=>"Geo:Geometry", "Geo:AsWKT"=>$wkt);

	if(array_key_exists("properties", $geo)) {
		foreach($geo["properties"] as $key=>$value) 
			$ngeo[$key] = $value;
	}
	return $ngeo;
}

function locationToJson($geo, $parent=null) {
	$geojson = array("geometry"=>array("coordinates"=>[]), "properties"=>array());
		
	if(array_key_exists("coordinates", $geo)) {
		$latlong = explode(",", $geo["coordinates"]);
		$geojson["geometry"]["coordinates"][0] = floatval($latlong[1]);
		$geojson["geometry"]["coordinates"][1] = floatval($latlong[0]);
		$geojson["geometry"]["type"] = "Point";
	}
	if(array_key_exists("wgs84:Long", $geo)) 
		$geojson["geometry"]["coordinates"][0] = floatval($geo["wgs84:Long"]);
	if(array_key_exists("wgs84:Lat", $geo)) 
		$geojson["geometry"]["coordinates"][1] = floatval($geo["wgs84:Lat"]);
	if(array_key_exists("wgs84:Alt", $geo)) 
		$geojson["geometry"]["coordinates"][2] = floatval($geo["wgs84:Alt"]);

	foreach($geo as $prop=>$value) {
		if($prop[0] == "@")
			continue;
		if($prop == "locationType") {
			$geojson["type"] = $geo["locationType"];
		}
		else if(($prop == "coordinates") || ($prop == "coordinatesFor")) {
			// Ignore
		}
		else if(preg_match("/^(geo|wgs84):/", $prop)) {
			// Ignore
		}
		else {
			$geojson["properties"][$prop] = $value;
		}
	}
	return $geojson;
}

function getUncertainty($val, $parent=null) {
	$uncertainty = array();
	$uncertainty["hasValue"] = $val;
	$uncertainty["analytical"] = $val;
	$uncertainty["reproducibility"] = $val;
	return $uncertainty;
}

function getGoogleSpreadsheetURL($key, $parent=null) {
	return "https://docs.google.com/spreadsheets/d/$key";
}

function getGoogleSpreadsheetKey($url, $parent=null) {
	return str_replace("https://docs.google.com/spreadsheets/d/", "", $url);
}

function getParentProperty($obj, $prop) {
	$parent = $obj["@parent"];
	while($parent) {
		if(array_key_exists($prop, $parent))
			return $parent[$prop];
		$parent = $parent["@parent"];
	}
	return null;
}

function getParentWithPropertyValue($obj, $prop, $val) {
	$parent = $obj["@parent"];
	while($parent) {
		if(array_key_exists($prop, $parent)
				&& $parent[$prop] == $val)
			return $parent;
		$parent = $parent["@parent"];
	}
	return null;
}

// Object json conversions

function setIdentifierProperties($pub, $hash) {
	$props = array();
	if(array_key_exists("identifier", $pub)) {
		foreach($pub["identifier"] as $identifier) {
			if($identifier["type"] == "doi") 
				$pub["hasDOI"][] = $identifier["id"];
			else if($identifier["type"] == "issn") 
				$pub["hasISSN"][] = $identifier["id"];
			else if($identifier["type"] == "isbn") 
				$pub["hasISBN"][] = $identifier["id"];
			if(array_key_exists("url", $identifier))
				$pub["hasLink"][] = $identifier["url"];
		}
		unset($pub["identifier"]);
	}
	return [$pub, $hash, array()];
}

function setVariableCategory($obj, $hash) {
	// Default category
	$obj["@category"] = "MeasuredVariable";
	$obj["@schema"] = "Variable";
	if(array_key_exists("variableType", $obj)) {
		$obj["@category"] = ucfirst($obj["variableType"]."Variable");
		unset($obj["variableType"]);
	}
	else if(array_key_exists("calibration", $obj)) {
		$obj["@category"] = "InferredVariable";
	}
	return [$obj, $hash, array()];
}

function getArchiveType($id, $latitude) {
	$id = strtolower($id);
	if($id == "tree")
		return "Wood";
	else if($id == "bivalve")
		return "MolluskShell";
	else if($id == "borehole") {
		if($latitude > 65 || $latitude < -65)
			return "GlacierIce";
		else
			return "Rock";
	}
	return camelCase($id);
}

function camelCase($id) {
	$term = "";
	foreach(preg_split("/\s+/", $id) as $subid)
		$term .= ucfirst($subid);
	return $term;
}

function guessSensorType($archive, $observation, $sensor) {
	if(array_key_exists('sensorGenus', $sensor) || array_key_exists('sensorSpecies', $sensor)) {
		if($archive == "MarineSediment")
			return "Foraminifera";
		else if($archive == "Coral")
			return "Polyp";
		else if($archive == "Wood")
			return "Vegetation";
		else if($archive == "MolluskShell")
			return "Bivalves";
		else if($archive == "Sclerosponge")
			return "Sponge";
		return "OrganicSensor";
	}
	else {
		if($archive == "MarineSediment" && ($observation == "Uk37" || $observation == "Alkenone"))
			$type = "Coccolithophores";
		else if($archive == "MarineSediment" && $observation == "TEX86")
			$type = "Archea";
		else if($archive == "MarineSediment" && $observation == "D18O")
			$type = "Foraminifera";
		else if($archive == "MarineSediment" && $observation == "Mg/Ca")
			$type = "Foraminifera";

		else if($archive == "LakeSediment" && ($observation == "Uk37" || $observation == "Alkenone"))
			$type = "Coccolithophores";
		else if($archive == "LakeSediment" && $observation == "TEX86")
			$type = "Archea";
		else if($archive == "LakeSediment" && $observation == "Midge")
			$type = "Chironomids";
		else if($archive == "LakeSediment" && $observation == "BSi")
			$type = "Diatoms";
		else if($archive == "LakeSediment" && $observation == "Chironomid")
			$type = "Chironomids";
		else if($archive == "LakeSediment" && $observation == "Reflectance")
			$type = "PhotosyntheticAlgae";
		else if($archive == "LakeSediment" && $observation == "Pollen")
			$type = "Watershed";

		else if($archive == "Coral")
			return "Polyp";
		else if($archive == "Wood")
			return "Vegetation";
		else if($archive == "MolluskShell")
			return "Bivalves";
		else if($archive == "Sclerosponge")
			return "Sponge";

		// Inorganic Sensors
		else if($archive == "Speleothem")
			return "Karst";
		else if($archive == "GlacierIce")
			return "Snow"; // TODO: Or Firn/Ice
		else if($archive == "LakeSediment" && $observation == "VarveThickness")
			return "Catchment";
		else if($archive == "GlacierIce" && $observation == "Melt")
			return "IceSurface";
		else if($archive == "Borehole")
			return "Soil"; // TODO: Or rock or ice
		else
			return "InorganicSensor";
	}
}

function getObservation($observation) {
	if(strtolower($observation) == "alkenone")
		return "Uk37";
	return camelCase($observation);
}

function checkForDepthVariable($obj, $hash) {
	$depthcol = null;
	foreach($obj["columns"] as $col) {
		if($col["variableName"] == "depth")
			$depthcol = $col["TSid"].".".$col["variableName"];
	}
	if($depthcol) {
		foreach($obj["columns"] as &$col) {
			$thiscol = $col["TSid"].".".$col["variableName"];
			if($thiscol != $depthcol)
				$col["takenAtDepth"] = $depthcol;
		}
	}
	return [$obj, $hash, array()];
}

function removeDepthProperty($val, $parent=null) {
	if(array_key_exists("takenAtDepth", $val))
		unset($val["takenAtDepth"]);
	return $val;
}


function createProxySystem($obj, $hash) {
	$varid = $obj["@id"];

	// Deal with proxies
	if(array_key_exists("proxy", $obj)) {
		// Get the archive type
		$dsname = getParentProperty($obj, "dataSetName");
		$geo = getParentProperty($obj, "geo");

		$latitude = 0;
		if(array_key_exists("geometry", $geo))
			$latitude = $geo["geometry"]["coordinates"][1];

		$archivetype = getParentProperty($obj, "archiveType");
		$archivetype = getArchiveType($archivetype, $latitude);
		$archiveid = $obj["@parent"]["@id"].".".$archivetype;

		// Create archive
		if(!array_key_exists($archiveid, $hash)) {
			$archive = array("@id"=>$archiveid, "@category"=>$archivetype);
			$archive["name"] = $archivetype;
			$hash[$archiveid] = $archive;
		}

		$observationid = getObservation($obj["proxy"]);

		// Create sensor
		$sensorid = $observationid."DefaultSensor";
		$sensor = array("@id"=>$sensorid, "@category"=>"Sensor");
		if(array_key_exists("archiveGenus", $obj)) {
			$sensor["sensorGenus"] = $obj["archiveGenus"];
			$sensorid = camelCase($sensor["sensorGenus"]);
			unset($obj["archiveGenus"]);
			if(array_key_exists("archiveSpecies", $obj)) {
				$sensor["sensorSpecies"] = $obj["archiveSpecies"];
				$sensorid .= "-" .camelCase($sensor["sensorSpecies"]);
				unset($obj["archiveSpecies"]);
			}
		}
		if(!array_key_exists($sensorid, $hash)) {
			$sensor["@id"] = $sensorid;
			$sensor["@category"] = guessSensorType($archivetype, $observationid, $sensor); 
			$hash[$sensorid] = $sensor;
		}

		$hash[$archiveid]["sensingWith"] = $sensorid;

		// Create a proxy
		$proxyid = $obj["@id"].".$archivetype.$sensorid.ProxySystem";
		if(!array_key_exists($proxyid, $hash)) {
			$proxy = array(
				"@id"=>$proxyid, 
				"@category"=>"ProxySystem",
				"name"=>$observationid, 
				"hasProxySensor"=>$sensorid, 
				"hasProxyArchive"=>$archiveid,
				"hasProxyObservation"=>$observationid
			);
			if(array_key_exists("proxySystemModel", $obj)) {
				$proxymodelid = "$proxyid.Model";
				// TODO: Create proxy sensor/archive/observation models
				$proxymodel = array(
					"@id"=>$proxymodelid, 
					"@category"=>"ProxySystemModel",
					"name"=>$observationid, 
					"hasProxySensorModel"=>"$sensorid.Model", 
					"hasProxyArchiveModel"=>"$archiveid.Model",
					"hasProxyObservationModel"=>"$observationid.Model"
				);
				$proxy["modeledBy"] = $proxymodelid;
				$hash[$proxymodelid] = $proxymodel;
				unset($obj["proxySystemModel"]);
			}
			$hash[$proxyid] = $proxy;
		}

		$obj["measuredOn"] = $archiveid;
		$obj["onProxyObservationProperty"] = $observationid;
		$obj["hasProxySystem"] = $proxyid;

		unset($obj["proxy"]);
		return [$obj, $hash, array($archiveid, $proxyid, $sensorid)];
	}
	return [$obj, $hash, array()];
}

function wrapIntegrationTime($obj, $hash) {
	$objid = $obj["@id"];

	// Deal with integrationTime
	$pvals = array();
	foreach($obj as $key=>$value) {
		if(preg_match("/^integrationTime$/i", $key)) {
			$pvals["hasValue"] = $value;
			unset($obj[$key]);
		}
		else if(preg_match("/^integrationTime(.+)/i", $key, $m)) {
			$pvals[lcfirst($m[1])] = $value;
			unset($obj[$key]);
		}
	}
	if(sizeof($pvals) > 0) {
		$intimeid = "$objid.IntegrationTime";
		$obj["integrationTime"] = $intimeid;
		$intime["@id"] = $intimeid;
		$intime["@category"] = "IntegrationTime";
		$intime["@schema"] = "IntegrationTime";
		$intime = array_merge($intime, $pvals);
		$hash[$intimeid] = $intime;
		return [$obj, $hash, array($intimeid)];
	}
	return [$obj, $hash, array()];
}

function wrapUncertainty($obj, $hash) {
	$objid = $obj["@id"];

	// Deal with uncertainty
	$pvals = array();
	foreach($obj as $key=>$value) {
		if(preg_match("/^uncertainty$/i", $key)) {
			$pvals["hasValue"] = $value;
			unset($obj[$key]);
		}
		else if(preg_match("/^uncertainty/i", $key)) {
			$pvals[$key] = $value;
			unset($obj[$key]);
		}
	}
	if(sizeof($pvals) > 0) {
		$uncid = "$objid.Uncertainty";
		$obj["hasUncertainty"] = $uncid;
		$uncertainty = array("@id"=>$uncid, "@category"=>"Uncertainty");
		foreach($pvals as $prop=>$value) {
			$uncertainty[$prop] = $value;
		}
		$hash[$uncid] = $uncertainty;

		return [$obj, $hash, array($uncid)];
	}
	return [$obj, $hash, array()];
}

function addFoundInTable($obj, $hash) {
	$obj["foundInTable"] = $obj["@parent"]["@id"];
	return [$obj, $hash, array()];
}

// Object json reverse conversion

function removeFoundInTable($var, $parent=null) {
	if(array_key_exists("foundInTable", $var))
		unset($var["foundInTable"]);
	return $var;
}

function extractFromProxySystem($var, $parent=null) {
	if(array_key_exists("hasProxySystem", $var)) {
		$ps = $var["hasProxySystem"];
		$var["proxy"] = $ps["name"];
		if(array_key_exists("hasProxySensor", $ps)) {
			if(array_key_exists("sensorGenus", $ps["hasProxySensor"]))
				$var["archiveGenus"] = $ps["hasProxySensor"]["sensorGenus"];
			if(array_key_exists("sensorSpecies", $ps["hasProxySensor"]))
				$var["archiveSpecies"] = $ps["hasProxySensor"]["sensorSpecies"];
		}
		unset($var["hasProxySystem"]);
		unset($var["onProxyObservationProperty"]);
		unset($var["measuredOn"]);
	}
	return $var;
}

function unwrapUncertainty($var, $parent=null) {
	if(array_key_exists("hasUncertainty", $var)) {
		$unc = $var["hasUncertainty"];
		if(array_key_exists("hasValue", $unc)) {
			$var["uncertainty"] = floatval($unc["hasValue"]);
			unset($unc["hasValue"]);
		}
		foreach($unc as $key=>$value) {
			if($key[0] != "@")
				$var[$key] = $value;
		}
		unset($var["hasUncertainty"]);
	}
	return $var;
}

function unwrapIntegrationTime($interp, $parent=null) {
	if(array_key_exists("integrationTime", $interp)) {
		$intime = $interp["integrationTime"];
		if(array_key_exists("hasValue", $intime)) {
			$interp["integrationTime"] = floatval($intime["hasValue"]);
			unset($intime["hasValue"]);
		}
		foreach($intime as $key=>$value) {
			if($key[0] != "@")
				$interp["integrationTime".ucfirst($key)] = $value;
		}
		unset($interp["hasIntegrationTime"]);
	}
	return $interp;
}

function collectVariablesById(&$item, $arr) {
	if(!is_array($item))
		return $arr;

	// Data is a Hash
	if(array_key_exists("@category", $item) &&
			array_key_exists("@id", $item) &&
			preg_match("/Variable$/", $item["@category"])) {
		$arr[$item["@id"]] = &$item;
	}
	else
		foreach($item as $key=>$value) {
			if($key[0] != "@")
				$arr = collectVariablesById($item[$key], $arr);
		}

	return $arr;
}

function setVariableType($var, $parent=null) {
	if($var["@category"] == "MeasuredVariable")
		$var["variableType"] = "measured";
	if($var["@category"] == "InferredVariable")
		$var["variableType"] = "inferred";
	return $var;
}

function createPublicationIdentifier($pub, $parent=null) {
	$identifiers = array();
	if(array_key_exists("hasDOI", $pub)) {
		$identifier["type"] = "doi";
		$identifier["id"] = $pub["hasDOI"]; 
		if(array_key_exists("link", $pub)) {
			foreach($pub["link"] as $link) {
				if(preg_match("/dx.doi.org/", $link)) {
					$identifier["url"] = $link;
				}
			}
			unset($pub["link"]);
		}
		unset($pub["hasDOI"]);
		$identifiers[] = $identifier;
	}
	$pub["identifier"] = $identifiers;
	return $pub;
}

function changeSeasonalityType($interp, $parent=null) {
	if(array_key_exists("seasonality", $interp)) {
		if(is_array($interp["seasonality"])) {
			$newseasonality = array();
			foreach($interp["seasonality"] as $svalue)
				$newseasonality[] = floatval($svalue);
			$interp["seasonality"] = $newseasonality;
		}
	}
	return $interp;
}

?>
