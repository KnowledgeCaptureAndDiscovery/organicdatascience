<?php 

class WTSummary{
	
	private $total_nrOfTasks = 0;
	private $meta_nrOfTasks = 0;
	private $content_nrOfTasks = 0;
	
	private $total_nrOfType = 0;
	private $meta_nrOfType = 0;
	private $content_nrOfType = 0;
	
	private $total_nrOfTypeHigh = 0;
	private $meta_nrOfTypeHigh = 0;
	private $content_nrOfTypeHigh = 0;
	
	private $total_nrOfTypeMedium = 0;
	private $meta_nrOfTypeMedium = 0;
	private $content_nrOfTypeMedium = 0;
	
	private $total_nrOfTypeLow = 0;
	private $meta_nrOfTypeLow = 0;
	private $content_nrOfTypeLow = 0;
	
	private $total_nrOfOwners = 0;
	private $meta_nrOfOwners = 0;
	private $content_nrOfOwners = 0;
		
	private $total_nrOfParticipats = 0;
	private $meta_nrOfParticipats = 0;
	private $content_nrOfParticipats = 0;
	
	private $total_absoluteNrOfParticipats = 0;
	private $meta_absoluteNrOfParticipats = 0;
	private $content_absoluteNrOfParticipats = 0;
	
	private $total_nrOfExpertise = 0;
	private $meta_nrOfExpertise = 0;
	private $content_nrOfExpertise = 0;
	
	private $total_absoluteNrOfExpertise = 0;
	private $meta_absoluteNrOfExpertise = 0;
	private $content_absoluteNrOfExpertise = 0;	
	
	private $total_nrOfStart = 0;
	private $meta_nrOfStart = 0;
	private $content_nrOfStart = 0;
	
	private $total_nrOfTarget = 0;
	private $meta_nrOfTarget = 0;
	private $content_nrOfTarget = 0;
	
	private $total_nrOfStartAndTarget = 0;
	private $meta_nrOfStartAndTarget = 0;
	private $content_nrOfStartAndTarget = 0;
	
	private $total_startTargetIntervalSum = 0;
	private $meta_startTargetIntervalSum = 0;
	private $content_startTargetIntervalSum = 0;
	
	private $total_nrOfProgress = 0;
	private $meta_nrOfProgress = 0;
	private $content_nrOfProgress = 0;
	
	private $total_progressSum = 0;
	private $meta_progressSum = 0;
	private $content_progressSum = 0;
	
	public function calculateSummary(&$tasks) {
				
		foreach($tasks as $k => $task){
			
			$this->total_nrOfTasks++;
			$this->total_nrOfType += self::nrOfType($task);	
			$this->total_nrOfTypeHigh += self::nrOfTypeHigh($task);	
			$this->total_nrOfTypeMedium += self::nrOfTypeMedium($task);	
			$this->total_nrOfTypeLow += self::nrOfTypeLow($task);	
			$this->total_nrOfOwners += self::nrOfOwners($task);			
			$this->total_nrOfProgress += self::nrOfProgress($task);
			$this->total_progressSum += self::progress($task);
			$this->total_nrOfStart += self::nrOfStart($task);
			$this->total_nrOfTarget += self::nrOfTarget($task);
			$this->total_nrOfStartAndTarget += self::nrOfStartAndTarget($task);
			$this->total_startTargetIntervalSum += self::startTargetIntervalInDays($task);
			$this->total_nrOfExpertise += self::nrOfExperise($task);
			$this->total_absoluteNrOfExpertise += self::absoluteNrOfExperise($task);			
			$this->total_nrOfParticipats += self::nrOfParticipants($task);
			$this->total_absoluteNrOfParticipats += self::absoluteNrOfParticipants($task);
		
			
			if(WTDashboardAnalyze::isOnMetalevel($task)){			
				$this->meta_nrOfTasks++;
				$this->meta_nrOfType += self::nrOfType($task);	
				$this->meta_nrOfTypeHigh += self::nrOfTypeHigh($task);	
				$this->meta_nrOfTypeMedium += self::nrOfTypeMedium($task);	
				$this->meta_nrOfTypeLow += self::nrOfTypeLow($task);
				$this->meta_nrOfOwners += self::nrOfOwners($task);			
				$this->meta_nrOfProgress += self::nrOfProgress($task);
				$this->meta_progressSum += self::progress($task);
				$this->meta_nrOfStart += self::nrOfStart($task);
				$this->meta_nrOfTarget += self::nrOfTarget($task);
				$this->meta_nrOfStartAndTarget += self::nrOfStartAndTarget($task);
				$this->meta_startTargetIntervalSum += self::startTargetIntervalInDays($task);
				$this->meta_nrOfExpertise += self::nrOfExperise($task);
				$this->meta_absoluteNrOfExpertise += self::absoluteNrOfExperise($task);			
				$this->meta_nrOfParticipats += self::nrOfParticipants($task);
				$this->meta_absoluteNrOfParticipats += self::absoluteNrOfParticipants($task);
			}else{
				$this->content_nrOfTasks++;
				$this->content_nrOfType += self::nrOfType($task);	
				$this->content_nrOfTypeHigh += self::nrOfTypeHigh($task);	
				$this->content_nrOfTypeMedium += self::nrOfTypeMedium($task);	
				$this->content_nrOfTypeLow += self::nrOfTypeLow($task);
				$this->content_nrOfOwners += self::nrOfOwners($task);			
				$this->content_nrOfProgress += self::nrOfProgress($task);
				$this->content_progressSum += self::progress($task);
				$this->content_nrOfStart += self::nrOfStart($task);
				$this->content_nrOfTarget += self::nrOfTarget($task);
				$this->content_nrOfStartAndTarget += self::nrOfStartAndTarget($task);
				$this->content_startTargetIntervalSum += self::startTargetIntervalInDays($task);
				$this->content_nrOfExpertise += self::nrOfExperise($task);
				$this->content_absoluteNrOfExpertise += self::absoluteNrOfExperise($task);			
				$this->content_nrOfParticipats += self::nrOfParticipants($task);
				$this->content_absoluteNrOfParticipats += self::absoluteNrOfParticipants($task);
			}			
		}		
	}
	
	
	private static function nrOfStart($task) {
		if($task['facts']['start'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function nrOfTarget($task) {
		if($task['facts']['target'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function nrOfStartAndTarget($task) {
		if($task['facts']['start'] != WTDashboardAnalyze::$u && $task['facts']['target'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function startTargetIntervalInDays($task) {
		if($task['facts']['start'] != WTDashboardAnalyze::$u && $task['facts']['target'] != WTDashboardAnalyze::$u)
			return ($task['facts']['target']-$task['facts']['start'])/(24*3600);
		return 0;
	}
	
	private static function nrOfProgress($task) {
		if($task['facts']['progress'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function nrOfOwners($task) {
		if($task['facts']['owner'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function nrOfType($task) {
		if($task['facts']['type'] != WTDashboardAnalyze::$u)
			return 1;
		return 0;
	}
	
	private static function nrOfTypeHigh($task) {
		if($task['facts']['type'] == 'high')
			return 1;
		return 0;
	}
	
	private static function nrOfTypeMedium($task) {
		if($task['facts']['type'] == 'medium')
			return 1;
		return 0;
	}
	
	private static function nrOfTypeLow($task) {
		if($task['facts']['type'] == 'low')
			return 1;
		return 0;
	}
	
	private static function nrOfParticipants($task) {
		foreach ($task['facts']['participants'] as $k => $p) 
			if($p != WTDashboardAnalyze::$u)
				return 1;
		return 0;
	}
	
	private static function absoluteNrOfParticipants($task) {
		$nrParticipants = 0;
		foreach ($task['facts']['participants'] as $k => $p) 
			if($p != WTDashboardAnalyze::$u)
				$nrParticipants++;
		return $nrParticipants;
	}
	
	private static function nrOfExperise($task) {
		foreach ($task['facts']['expertise'] as $k => $p) 
			if($p != WTDashboardAnalyze::$u)
				return 1;
		return 0;
	}
	
	private static function absoluteNrOfExperise($task) {
		$nrOfExperise = 0;
		foreach ($task['facts']['expertise'] as $k => $p) 
			if($p != WTDashboardAnalyze::$u)
				$nrOfExperise++;
		return $nrOfExperise;
	}

	private static function progress($task) {
		if($task['facts']['progress'] != WTDashboardAnalyze::$u)
			return $task['facts']['progress'];
		return 0;
	}

		
	public function toJson() {
		return array(
			'total' => array(
								'nrOfTasks' => $this->total_nrOfTasks,
				'percentageOfType' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfType/$this->total_nrOfTasks,	
				'avgTypeHigh' => 
					$this->total_nrOfType == 0 ? 0:
					 	$this->total_nrOfTypeHigh/$this->total_nrOfType,	
				'avgTypeMedium' => 
					 $this->total_nrOfType == 0 ? 0:
					 	$this->total_nrOfTypeMedium/$this->total_nrOfType,	
				'avgTypeLow' => 
					 $this->total_nrOfType == 0 ? 0:	
					 	$this->total_nrOfTypeLow/$this->total_nrOfType,	
				'percentageOfProgress' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfProgress/$this->total_nrOfTasks,			
				'avgProgress' => 
					$this->total_nrOfProgress == 0 ? 0:
						$this->total_progressSum/$this->total_nrOfProgress,
				'percentageOfStart' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfStart/$this->total_nrOfTasks,
				'percentageOfTarget' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfTarget/$this->total_nrOfTasks,
				'percentageOfStartAndTarget' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfStartAndTarget/$this->total_nrOfTasks,
				'avgStartTargetInvalInDays' => 
					$this->total_nrOfStartAndTarget == 0 ? 0:
						$this->total_startTargetIntervalSum/$this->total_nrOfStartAndTarget,
				'percentageOfOwners' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfOwners/$this->total_nrOfTasks,					
				'percentageOfParticipants' => 
					$this->total_nrOfTasks == 0 ? 0:
						$this->total_nrOfParticipats/$this->total_nrOfTasks,
				'avgNrOfParticipants' => 
					$this->total_nrOfParticipats == 0 ? 0: 
						$this->total_absoluteNrOfParticipats/$this->total_nrOfParticipats,
				'percentageOfExpertise' => 
					$this->total_nrOfTasks == 0 ? 0: 	
						$this->total_nrOfExpertise/$this->total_nrOfTasks,
				'avgNrOfExpertise' => 
					$this->total_nrOfExpertise == 0 ? 0:
						$this->total_absoluteNrOfExpertise/$this->total_nrOfExpertise				
				),
			'meta' => array(
				'nrOfTasks' => $this->meta_nrOfTasks,
				'percentageOfType' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfType/$this->meta_nrOfTasks,	
				'avgTypeHigh' => 
					$this->meta_nrOfType == 0 ? 0:
					 	$this->meta_nrOfTypeHigh/$this->meta_nrOfType,	
				'avgTypeMedium' => 
					 $this->meta_nrOfType == 0 ? 0:
					 	$this->meta_nrOfTypeMedium/$this->meta_nrOfType,	
				'avgTypeLow' => 
					 $this->meta_nrOfType == 0 ? 0:	
					 	$this->meta_nrOfTypeLow/$this->meta_nrOfType,	
				'percentageOfProgress' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfProgress/$this->meta_nrOfTasks,			
				'avgProgress' => 
					$this->meta_nrOfProgress == 0 ? 0:
						$this->meta_progressSum/$this->meta_nrOfProgress,
				'percentageOfStart' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfStart/$this->meta_nrOfTasks,
				'percentageOfTarget' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfTarget/$this->meta_nrOfTasks,
				'percentageOfStartAndTarget' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfStartAndTarget/$this->meta_nrOfTasks,
				'avgStartTargetInvalInDays' => 
					$this->meta_nrOfStartAndTarget == 0 ? 0:
						$this->meta_startTargetIntervalSum/$this->meta_nrOfStartAndTarget,
				'percentageOfOwners' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfOwners/$this->meta_nrOfTasks,					
				'percentageOfParticipants' => 
					$this->meta_nrOfTasks == 0 ? 0:
						$this->meta_nrOfParticipats/$this->meta_nrOfTasks,
				'avgNrOfParticipants' => 
					$this->meta_nrOfParticipats == 0 ? 0: 
						$this->meta_absoluteNrOfParticipats/$this->meta_nrOfParticipats,
				'percentageOfExpertise' => 
					$this->meta_nrOfTasks == 0 ? 0: 	
						$this->meta_nrOfExpertise/$this->meta_nrOfTasks,
				'avgNrOfExpertise' => 
					$this->meta_nrOfExpertise == 0 ? 0:
						$this->meta_absoluteNrOfExpertise/$this->meta_nrOfExpertise		
				),
			'content' => array(
								'nrOfTasks' => $this->content_nrOfTasks,
				'percentageOfType' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfType/$this->content_nrOfTasks,	
				'avgTypeHigh' => 
					$this->content_nrOfType == 0 ? 0:
					 	$this->content_nrOfTypeHigh/$this->content_nrOfType,	
				'avgTypeMedium' => 
					 $this->content_nrOfType == 0 ? 0:
					 	$this->content_nrOfTypeMedium/$this->content_nrOfType,	
				'avgTypeLow' => 
					 $this->content_nrOfType == 0 ? 0:	
					 	$this->content_nrOfTypeLow/$this->content_nrOfType,	
				'percentageOfProgress' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfProgress/$this->content_nrOfTasks,			
				'avgProgress' => 
					$this->content_nrOfProgress == 0 ? 0:
						$this->content_progressSum/$this->content_nrOfProgress,
				'percentageOfStart' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfStart/$this->content_nrOfTasks,
				'percentageOfTarget' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfTarget/$this->content_nrOfTasks,
				'percentageOfStartAndTarget' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfStartAndTarget/$this->content_nrOfTasks,
				'avgStartTargetInvalInDays' => 
					$this->content_nrOfStartAndTarget == 0 ? 0:
						$this->content_startTargetIntervalSum/$this->content_nrOfStartAndTarget,
				'percentageOfOwners' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfOwners/$this->content_nrOfTasks,					
				'percentageOfParticipants' => 
					$this->content_nrOfTasks == 0 ? 0:
						$this->content_nrOfParticipats/$this->content_nrOfTasks,
				'avgNrOfParticipants' => 
					$this->content_nrOfParticipats == 0 ? 0: 
						$this->content_absoluteNrOfParticipats/$this->content_nrOfParticipats,
				'percentageOfExpertise' => 
					$this->content_nrOfTasks == 0 ? 0: 	
						$this->content_nrOfExpertise/$this->content_nrOfTasks,
				'avgNrOfExpertise' => 
					$this->content_nrOfExpertise == 0 ? 0:
						$this->content_absoluteNrOfExpertise/$this->content_nrOfExpertise			
				));	
	}	
	
	
}