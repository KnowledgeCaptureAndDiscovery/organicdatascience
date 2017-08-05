<?php

global $SCHEMA, $BLACKLIST;

$BLACKLIST = array(
	'metadataMD5'=>1,
	'paleoData_paleoDataMD5'=>1,
	'paleoData_paleoMeasurementTableMD5'=>1,
	'paleoDataMD5'=>1,
	'paleoMeasurementTableMD5'=>1,
	'tagMD5'=>1,
	'chronData_chronDataMD5'=>1,
	'chronData_chronMeasurementTableMD5'=>1,
	'chronDataMD5'=>1,
	'chronMeasurementTableMD5'=>1,
	'earliestSampleDate'=>1,
	'latestSampleDate'=>1,
	'inCompilation'=>1
);

$SCHEMA["Dataset"] = array(
	"@id" => array("{dataSetName}"),
	"@fromJson" => array("addExtraDatasetProperties"),
	"@toJson" => array("getVariableArchiveTypes"),

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
		"alternates"=>["archive", "paleoDataArchive", "paleoData_Archive"]
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
	"@id" => array("Publication.", "{identifier.0.id|doi|hasDOI|@parent.dataSetName}"),
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
	"@fromJson" => array("setInterVariableLinks"),

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
	"@id" => array("{TSid|tsid|foundInTable|@parent.@id}", ".", "{variableName|name}"),
	"@fromJson" => array("setVariableCategory", "wrapUncertainty", "createProxySystem", "addFoundInTable"),
	"@toJson" => array("setVariableType", "unwrapUncertainty", "extractFromProxySystem", "removeFoundInTable", "removeDepthProperty"),

	"number" => array("name"=>"HasColumnNumber", "type"=>"Number"),
	"TSid" => array("name"=>"HasVariableID", "alternates"=>["tsid"]),
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
		"name"=>"CalibratedVia",
		"schema"=>"CalibrationModel",
		"multiple"=>true
	),
	"interpretation" => array(
		"name"=>"InterpretedAs",
		"schema"=>"Interpretation",
		"category"=>"Interpretation",
		"multiple"=>true
	),
	"resolution" => array(
		"name"=>"HasResolution",
		"category"=>"Resolution",
		"schema"=>"Resolution",
		"alternates"=>["hasResolution"]
	),
	"inferredFrom" => array(
		"schema"=>"Variable",
		"category"=>"MeasuredVariable"
	),
	"hasUncertainty" => array(
		"schema"=>"Uncertainty",
		"multiple"=>true
	),
	"useInGlobalTemperatureAnalysis" => array(
		"name"=>"UseInPAGES2kGlobalTemperatureAnalysis"
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

$SCHEMA["PhysicalSample"] = array(
	"hasidentifier" => array( "name"=> "hasIGSN" ),
	"hasname" => array("name" => "Name" ),
	"housedat" => array("name"=> "HousedAt")
);

$SCHEMA["Resolution"] = array(
	"@id" => array("{@parent.@id}", ".Resolution"),
	"@fromJson" => array("valuesToString"),
	"@toJson" => array("valuesToArray")
);

$SCHEMA["Location"] = array(
	"@id" => array("{@parent.dataSetName}", ".Location"),

	"siteName" => array( "name"=>"Name"),
	"coordinates" => array( "type"=>"Geographic_coordinate" ),
	"coordinatesFor" => array( "type"=>"Page" )
);

$SCHEMA["Interpretation"] = array(
	"@id" => array("{@parent.@id}", ".Interpretation", "{@index}"),
	"@toJson" => array("changeSeasonalityType"),

	"interpDirection" => array("name"=>"InterpretationDirection", "alternates"=>["dir", "interpDir", "interpdirection", "direction"]),
	"variable" => array("name"=>"Name"),
	"variableDetail" => array("name"=>"Detail", "alternates"=>["variabledetail"]),
	"rank" => array("name"=>"hasRank"),
	"basis"=> array("name"=>"RelevantQuote"),
	"local"=> array("name"=>"IsLocal")
);

$SCHEMA["IsotopeInterpretation"] = array(
	"@id" => array("{@parent.@id}", ".IsotopeInterpretation", "{@index}"),
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
	"@id" => array("{@parent.@id}", ".Calibration"),
	"@fromJson" => array("wrapUncertainty"),
	"@toJson" => array("unwrapUncertainty"),
	"reference"=> array("name"=>"RelevantQuote"),
	/*"reference"=>array(
		"name"=>"CalibrationReferences",
		"schema"=>"Publication",
		"alternates"=>"calibrationReferences"
	)*/
);

$SCHEMA["Person"] = array(
	"@id" => array("{name}")
);

$SCHEMA["Uncertainty"] = array(
	"@id" => array("{@parent.@id}", ".Uncertainty", "{@index}")
);


function addExtraDatasetProperties($obj, $hash) {
	// HACK
	foreach($_REQUEST as $key=>$value) {
		if(preg_match("/^extra_(.+)/", $key, $m)) {
			$prop = $m[1];
			if(!array_key_exists($prop, $obj)) {
				$obj[$prop] = $value;
			}
		}
	}
	return [$obj, $hash, array()];
}

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
	$coords = $geo["geometry"]["coordinates"];
	if($coords && sizeof($coords) > 0) {
		$ngeo["coordinates"] = $coords[1].",".$coords[0];
		$ngeo["Wgs84:Lat"] = $coords[1];
		$ngeo["Wgs84:Long"] = $coords[0];
		// FIXME: For now assuming points
		$wkt = "POINT(". $coords[1]." ".$coords[0];
		if(sizeof($coords) > 2) {
			$ngeo["Wgs84:Alt"] = $coords[2];
			$wkt .= " ".$coords[2];
		}
		$wkt .= ")";
		$ngeo["Geo:HasGeometry"] = array("@id"=>$parent["@id"].".Geometry", "@category"=>"Geo:Geometry", "Geo:AsWKT"=>$wkt);
	}
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

function valuesToString($obj, $hash) {
	if(array_key_exists("values", $obj)) {
		if(is_array($obj["values"]))
			$obj["values"] = implode(", ", $obj["values"]);
	}
	return [$obj, $hash, array()];
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

function getLiPDArchiveType($archiveType) {
	return unCamelCase($archiveType);
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

function unCamelCase($id) {
	$re = '/(?<=[a-z])(?=[A-Z]) | (?<=[A-Z])(?=[A-Z][a-z])/x';
	$a = preg_split($re, $id);
	return strtolower(implode(" " , $a));
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

function getVariableId($obj, $parentid) {
	$iobj = array_change_key_case($obj);
	$id = array_key_exists("tsid", $iobj) ? $iobj["tsid"] : $parentid;
	$id .= "." . $iobj["variablename"];
	return $id;
}

function setInterVariableLinks($obj, $hash) {
	$depthcol = null;
	$vhash = array();
	foreach($obj["columns"] as $col) {
		$vhash[strtolower($col["variableName"])] =  getVariableId($col, $obj["@id"]);
	}
	$depthcol = array_key_exists("depth", $vhash) ? $vhash["depth"] : null;
	foreach($obj["columns"] as &$col) {
		$thiscol = getVariableId($col, $obj["@id"]);
		if(array_key_exists("InferredFrom", $col)) {
			$infcol = strtolower($col["InferredFrom"]);
			if(array_key_exists($infcol, $vhash))
				$col["InferredFrom"] = $vhash[$infcol];
		}
		if($depthcol && $thiscol != $depthcol)
			$col["TakenAtDepth"] = $depthcol;
	}
	return [$obj, $hash, array()];
}

function removeDepthProperty($val, $parent=null) {
	if(array_key_exists("TakenAtDepth", $val))
		unset($val["TakenAtDepth"]);
	return $val;
}

function createProxySystem($obj, $hash) {
	$varid = $obj["@id"];

	// Deal with proxies
	$proxyobs = null;
	$sampleid = null;
	if(array_key_exists("proxy", $obj)) {
		$proxyobs = $obj["proxy"];
		unset($obj["proxy"]);
	}
	else if(array_key_exists("OnProxyObservationProperty", $obj)) {
		$proxyobs = $obj["OnProxyObservationProperty"];
		unset($obj["OnProxyObservationProperty"]);
	}
	else if(array_key_exists("ProxyObservationType", $obj))
		$proxyobs = $obj["ProxyObservationType"];

	$vartype = $obj["@category"];
	if($vartype && $vartype == "MeasuredVariable") {
	//if(array_key_exists("proxy", $obj)) {
		// Get the archive type
		$dsname = getParentProperty($obj, "dataSetName");
		$geo = getParentProperty($obj, "geo");

		$latitude = 0;
		if(array_key_exists("geometry", $geo) && sizeof($geo["geometry"]["coordinates"]) > 1)
			$latitude = $geo["geometry"]["coordinates"][1];

		$archivetype = getParentProperty($obj, "archiveType");
		if(!$archivetype)
			$archivetype = getParentProperty($obj, "archive");
		$archivetype = getArchiveType($archivetype, $latitude);

		// Create sample (archive)
		if(!array_key_exists("physicalSample", $obj)) {
			$cname = getParentProperty($obj, "collectionName");
			if($cname)
				$obj["physicalSample"] = array("name"=>$cname);
		}
		if(array_key_exists("physicalSample", $obj)) {
			$sample = $obj["physicalSample"];
			$sampleid = array_key_exists("hasname", $sample) ? $sample["hasname"] : $sample["name"];
			if(array_key_exists("hasidentifier", $sample)) {
				$sampleid .= ".". $sample["hasidentifier"];
			}
			else if(array_key_exists("identifier", $sample)) {
				$sampleid .= ".". $sample["identifier"];
			}
			if(!array_key_exists($sampleid, $hash)) {
				$sampleobj = array("@id"=>$sampleid,
					"@category"=>"PhysicalSample",
					"@extracats"=>[$archivetype]);

				foreach($sample as $pkey=>$pval)
					$sampleobj[$pkey] = $pval;

				$hash[$sampleid] = $sampleobj;
			}
			unset($obj["physicalSample"]);
		}

		$observationid = getObservation($proxyobs); //$obj["proxy"]);

		// Create sensor
		$sensorid = $observationid."DefaultSensor";
		$sensor = array("@id"=>$sensorid, "@category"=>"Sensor");
		if(array_key_exists("archiveGenus", $obj)) {
			$sensor["sensorGenus"] = $obj["archiveGenus"];
			$sensorid = ucfirst(strtolower($sensor["sensorGenus"]));
			unset($obj["archiveGenus"]);
			if(array_key_exists("archiveSpecies", $obj)) {
				$sensor["sensorSpecies"] = $obj["archiveSpecies"];
				$sensorid .= " " .strtolower($sensor["sensorSpecies"]);
				unset($obj["archiveSpecies"]);
			}
		}
		if(array_key_exists("sensorGenus", $obj)) {
			$sensor["sensorGenus"] = $obj["sensorGenus"];
			$sensorid = ucfirst(strtolower($sensor["sensorGenus"]));
			unset($obj["sensorGenus"]);
			if(array_key_exists("sensorSpecies", $obj)) {
				$sensor["sensorSpecies"] = $obj["sensorSpecies"];
				$sensorid .= " " .strtolower($sensor["sensorSpecies"]);
				unset($obj["sensorSpecies"]);
			}
		}
		if(!array_key_exists($sensorid, $hash)) {
			$sensor["@id"] = $sensorid;
			$sensor["@category"] = guessSensorType($archivetype, $observationid, $sensor);
			$hash[$sensorid] = $sensor;
		}

		//$hash[$sampleid]["ProxySensorType"] = $sensorid;

		// Create a proxy
		//$proxyid = $obj["@id"].".$archivetype.$sensorid.ProxySystem";
		$proxyid = "ProxySystem." . $archivetype;
		if($sensorid)
			$proxyid .= ".$sensorid";
		if($observationid)
			$proxyid .= ".$observationid";

		// TODO: $proxyid .= ".$chronmodel";
		// TODO: $proxyid .= ".$paleomodel";

		if(!array_key_exists($proxyid, $hash)) {
			$proxy = array(
				"@id"=>$proxyid,
				"@category"=>"ProxySystem",
				//"name"=>$observationid,
				"ProxySensorType"=>$sensorid,
				"ProxyArchiveType"=>$archivetype,
				"ProxyObservationType"=>$observationid
			);
			if(array_key_exists("proxySystemModel", $obj)) {
				$proxymodelid = "$proxyid.Model";
				// TODO: Create proxy sensor/archive/observation models
				$proxymodel = array(
					"@id"=>$proxymodelid,
					"@category"=>"ProxySystemModel",
					"name"=>$observationid,
					"hasProxySensorModel"=>"$sensorid.Model",
					"hasProxyArchiveModel"=>"$archivetype.Model",
					"hasProxyObservationModel"=>"$observationid.Model"
				);
				$proxy["modeledBy"] = $proxymodelid;
				$hash[$proxymodelid] = $proxymodel;
				unset($obj["proxySystemModel"]);
			}
			$hash[$proxyid] = $proxy;
		}

		$obj["measuredOn"] = $sampleid;
		$obj["ProxyObservationType"] = $observationid;
		$obj["hasProxySystem"] = $proxyid;

		unset($obj["proxy"]);
		return [$obj, $hash, array($sampleid, $proxyid, $sensorid)];
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

function _getVariableArchiveTypes(&$item, $atypes=array()) {
	$nitem = array();
	if(Util::is_assoc($item)) {
		foreach($item as $key=>&$value) {
			if($key == "archiveType") {
				$atypes[$value] = 1;
			}
			else {
				list($nitem[$key], $atypes) = _getVariableArchiveTypes($value, $atypes);
			}
		}
	}
	else if(is_array($item)) {
		foreach($item as &$value) {
			list($nit , $atypes) = _getVariableArchiveTypes($value, $atypes);
			$nitem[] = $nit;
		}
	}
	else {
		$nitem = $item;
	}
	return array($nitem, $atypes);
}

function getVariableArchiveTypes($var, $parent=null) {
	list($var, $atypes) = _getVariableArchiveTypes($var);
	foreach($atypes as $atype=>$ok) {
		$var["archiveType"] = $atype;
	}
	return $var;
}

function extractFromProxySystem($var, $parent=null) {
	if(array_key_exists("hasProxySystem", $var)) {
		$ps = $var["hasProxySystem"];
		//$var["proxy"] = $ps["name"];
		if(array_key_exists("hasProxySensor", $ps)) {
			$psensor = $ps["hasProxySensor"];
			if(is_object($psensor)) {
				if(array_key_exists("sensorGenus", $psensor))
					$var["sensorGenus"] = $psensor["sensorGenus"];
				if(array_key_exists("sensorSpecies", $psensor))
					$var["sensorSpecies"] = $psensor["sensorSpecies"];
			}
		}
		unset($var["hasProxySystem"]);
	}
	if(array_key_exists("measuredOn", $var)) {
		$archive = $var["measuredOn"];
		$archiveType = $archive["@category"];
		$var["archiveType"] = getLiPDArchiveType($archiveType);
		//print "$archiveType\n"
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

function valuesToArray($resolution, $parent=null) {
	if(array_key_exists("values", $resolution))
		return explode(",", $resolution["values"]);
}
?>
