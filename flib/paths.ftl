#-----------------------------------------------------------------------------
# script paths
#-----------------------------------------------------------------------------

set basename[path]:{
   .pel = split sys.fs.sep path!;
   .elcount = len pel!;
   if (elcount gt 0) { pel.(elcount-1) } { "" }!
}

set dirname[path]:{
   .pel = split sys.fs.sep path!;
   .elcount = len pel!;
   if (elcount gt 1) {
      pel.(elcount-1)=NULL; join sys.fs.sep pel!
   } {
      sys.fs.thisdir
   }!
}

set path_startwith[path,dir]:{
   .plen = len path!;
   if plen == 0 { dir } { join sys.shell.pathsep <dir, path>! }!
}

set path_addstart[pathenv,dir]:{
   .path = if (inenv sys.env pathenv!) {sys.env.(pathenv)}{""}!;
   sys.env.(pathenv) = path_startwith path dir!;
   # printf "env %s now '%s'\n"<pathenv,sys.env.(pathenv)>!;
}

set dir_bin dirname (sys.shell.self!)!

#path_addstart "PATH" dir_bin
path_addstart ""+(toupper (parse.codeid!)!)+"_PATH" dir_bin
path_addstart ""+(toupper (parse.codeid!)!)+"_PATH" "."
