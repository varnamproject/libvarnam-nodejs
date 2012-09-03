{
  "targets": [
    {
      "target_name": "varnam",
      "sources": [ "src/varnamjs.cc" ],
    	'libraries':[
        '-lvarnam'
      ],
      'include_dirs': ['<!(echo $VARNAM_INCLUDE_DIR)']
    }
  ]
}
