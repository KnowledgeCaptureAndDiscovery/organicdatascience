<?php
require_once 'WTTrackDB.php';

class WTTrack{
	
	public static function track($component, $subcomponent, $action, $actiontype, $taskId, $value, $user, $domain, $winX, $winY, $screenX, $screenY, $ip, $agent){		
		$con = WTTrackDB::connect();       
		$sql  = 'INSERT INTO track (component, subcomponent, action, actiontype, taskId, value, user, domain, winx, winy, screenx, screeny, ip, time) ';
		$sql .= 'VALUES (:component, :subcomponent, :action, :actiontype, :taskId, :value, :user, :domain, :winx, :winy, :screenx, :screeny, :ip, NOW())'; 
		$stmt = $con->prepare($sql);
		$stmt->bindValue(':component', $component);
		$stmt->bindValue(':subcomponent', $subcomponent);
		$stmt->bindValue(':action', $action);
		$stmt->bindValue(':actiontype', $actiontype);
		$stmt->bindValue(':value', $value);
		$stmt->bindValue(':taskId', $taskId);
		$stmt->bindValue(':winx', $winX);
		$stmt->bindValue(':winy', $winY);
		$stmt->bindValue(':screenx', $screenX);
		$stmt->bindValue(':screeny', $screenY);
		$stmt->bindValue(':user', $user);
		$stmt->bindValue(':domain', $domain);
		$stmt->bindValue(':ip', $ip);
		return $stmt->execute();
	}
	
	public static function readAll(){		
		$con = WTTrackDB::connect();      
		$result = $con->query('SELECT * FROM track ORDER BY time DESC LIMIT 100');
 
		$tracks = array();
		while($r = $result->fetch(PDO::FETCH_ASSOC)) {
			$track = array();
			$track['component'] = $r['component'];
			$track['subcomponent'] = $r['subcomponent'];
			$track['action'] = $r['action'];
			$track['actiontype'] = $r['actiontype'];
			$track['taskId'] = $r['taskId'];
			$track['value'] = $r['value'];			
			$track['user'] = $r['user'];
			$track['time'] = $r['time'];
			$track['domain'] = $r['domain'];
			$tracks[]= $track;
		}	
		return $tracks;
	}
	
	public static function delAll(){
//		$con = WTTrackDB::connect();      
//		$con->query('DELETE FROM track WHERE id>1');
//		return true;
	}
	
}