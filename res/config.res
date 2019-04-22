

code{str} = gb936 # some comment

display
	size{int, int} = 640, 480
	width{int} = 640
	height{int} = 480
	title{str} = '中文名字'
	ok = 0
	model
		name{str} = codes

		list = [1, 2, 3, "str"]
		list{int, int, int, str} = [1,2,3,"str"]
