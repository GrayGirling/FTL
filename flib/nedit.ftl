set codeid "nedit"

#-----------------------------------------------------------------------------
#
# whole file operations
#
#-----------------------------------------------------------------------------

set file_loaddata_max[openfn, maxsz, file]:{
    .data = NULL;
    .opened = openfn file "r"!;
    opened != NULL {
        data = io.read opened maxsz!;
        io.close opened!;
    }!;
    data
}


set file_savedata[openfn,file,data]:{
    .ok = FALSE;
    .opened = openfn file "w"!;
    opened != NULL {
        ok = io.write opened data!;
        io.close opened!;
    }!;
    ok
}

set loaddata file_loaddata_max io.binfile 10000000
set savedata file_savedata io.binfile

set loadtext file_loaddata_max io.file 10000000
set savetext file_savedata io.file


#-----------------------------------------------------------------------------
#
# line editor
#
#-----------------------------------------------------------------------------

set nedit[]:{
    .fname = NULL;
    .ntext = <>;
    .flistf = [fmt,outf]:{
        forall ntext [nt,n]:{ io.fprintf outf fmt [n=nt.0, text=nt.1]!; }!;
    };
    .flisttest = [fmt,outf]:{
        forall ntext [nt,n]:{ io.fprintf outf "%v\n" [n=n,text=nt]!; }!;
    };
    .clear = []:{ ntext=<>; };
    .addn = [n,line]:{
        .ix = NULL;
        .entry = select [nline]:{n == nline.0} ntext!;
        if entry == NULL {
            ntext.(len ntext!) = <n,line>;
            ntext = ntext.(sortby [nt1,nt2]:{nt1.0 - nt2.0 } ntext!);
        }{  ntext.((domain entry!).0) = <n,line>; }!;
    };
    .add = [nline]:{
        .pobj = parse.scan nline!;
        .n = NULL;
        parse.scanwhite pobj!;
        not (parse.scanempty pobj!) {
            if (parse.scanint @n pobj! {parse.scanwhite pobj!}!) {
                .line = parse.scanned pobj!;
                addn n line!
            }{io.fprintf io.err "Line must begin with an integer - '%s'\n"<nline>!}!;
        }!
    };
    nextln = [first, inc]:{
        .lns = len ntext!;
        .nxt = first;
        lns _gt_ 0 { nxt = ntext.(lns-1).0 + inc }!;
        nxt
    };
    lnread = [linedtext]:{
        .lines = split "\n" linedtext!;
        for lines [line]:{ add line!; }!;
    };
    read = [first,inc,linedtext]:{
        .lines = split "\n" linedtext!;
        .nxt = first;
        for lines [line]:{ addn nxt line!; nxt=nxt+inc; }!;
    };
    load = [file]:{
        clear!;
        .txt = loadtext file!;
        if txt != NULL {
            lnread txt!;
            fname = file;
        }{ io.fprintf io.err "Can't read file '%s'\n"<file>!; }!;
    };
    saveas = [file]:{
        if file == NULL { io.fprintf io.err "No filename set to save to\n"! }{
            .str = io.file file "w"!;
            if str != NULL {
                flistf "%[n]4d %[text]s\n" str!;
                io.close str!;
            }{ io.fprintf io.err "Can't write file '%s'\n"<file>!; }!;
        }!
    };
    .rename = [first,inc]:{
        .tlns = <>;
        .nxt = first;
        forall ntext [nt,n]:{ tlns.(len tlns!) = <nxt,nt.1>; nxt=nxt+inc; }!;
        ntext = tlns;
    };
    [ new = clear,
      add = add,
      addn = addn,
      ren = rename 10 10,
      next = nextln 10 10,
      read = read 10 10,
      append = [text]:{read (nextln 10 10!) 10 text!},
      lnread = lnread,
      load = load,
      save = []:{saveas fname!},
      saveas = saveas,
      flist = flistf "%[n]4d %[text]s\n",
      list = flistf "%[n]4d %[text]s\n" io.out,
      lntext = []:{ io.outstring [outf]:{flistf "%[n]d %[text]s\n" outf!}! },
      text = []:{ io.outstring [outf]:{flistf "%[text]s\n" outf!}! },
      file = []:{fname},
      # all = []:{ntext},
    ]
}


# to create new editors use
#   set g (new nedit!)!
#   set h (new nedit!)!
# to provide two editors g and h


set editor nedit!


#-----------------------------------------------------------------------------
#
# Default command handling
#
#-----------------------------------------------------------------------------


set get_item[out_item, line]:{
    .po = parse.scan line!;
    parse.scanwhite po!;
    parse.scanitemstr out_item po! {parse.scanempty po!}!
}


set unknown_command[line]:{
    .po = parse.scan line!;
    .n = NULL;
    parse.scanwhite po!;
    if (parse.scanint @n po!) {editor.add line!;}{
        io.fprintf io.err "%s: unrecognized command line %v\n" <codeid, line>!;
        NULL
    }!
}

set parse.on_badcmd (cmd unknown_command "- unknown command handler"!)

set load_fn[_help="<filename> - load file into the editor",line]:{
    .filename = NULL;
    if (get_item @filename line!) { editor.load filename! }{
        io.fprintf io.err "%s: syntax - %s\n" <codeid, _help>!;
    }!;
}

set saveas_fn[_help="<filename> - save lines to a file",line]:{
    .filename = NULL;
    if (get_item @filename line!) { editor.saveas filename! }{
        io.fprintf io.err "%s: syntax - %s\n" <codeid, _help>!;
    }!;
}

enter [
    new = (editor.new)::[_help="- clear lines in the editor"],
    ren = (editor.ren)::[_help="- renumber the lines in the editor"],
    load = cmd load_fn load_fn._help!,
    save = (editor.save)::[_help="- save lines back to the file they were loaded from"],
    saveas = cmd saveas_fn save_fn._help!,
    list = (editor.list)::[_help="- list the lines being edited"],
    file = editor.file,
    sh = sys.run,
]
