function get
{
	local n=$1
	if [ n -lt 10 ]; then
		return "000$n"
	fi	
	return "00$n"	
}
mv (get "1").pcl (get "1").cl
