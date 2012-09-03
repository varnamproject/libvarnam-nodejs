{
  "targets": [
    {
      "target_name": "varnam",
      "sources": [ "src/varnamjs.cc" ],
      'include_dirs': ['<!(echo $VARNAM_INCLUDE_DIR)'],
    	'link_settings': {
          'ldflags': ['-L<!(echo $VARNAM_LIB_DIR)', '-lvarnam']
      }
    }
  ]
}
