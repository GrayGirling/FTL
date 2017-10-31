#-----------------------------------------------------------------------------
#
# Register fields
#
#-----------------------------------------------------------------------------

set bitmask shiftl 1
set fieldmask[_help="<lsb> <width> - return mask for field",
              lsb,width]:{shiftl (shiftl 1 width!)-1 lsb! }

# the bit mask of a field
set maskoffield[fieldmask=fieldmask,field]::{fieldmask field.0 field.1!}

# the bit mask corresponding to a set of fields
set maskoffields[fieldvec]:{
   .fmask = 0;
   forall fieldvec [f]: {fmask = bitor fmask (fieldmask f.0 f.1!)!}!;
   fmask
}

# field: 0 - lsb,  1 - width
# mv:    0 - mask, 1 - value for mask
set fieldmv[_help="<field> <value> - create <mask, value> from field",
            field,value]:{
   .fm = fieldmask field.0 field.1!;
   <fm, bitand (shiftl value field.0!) fm!>
}

# find the mask,value corresponding to a set of mask,value pairs
# (for non-overlapping masks)
set fieldmv_sum[_help="<vec of fields> - create <mask, value> from fields",
                mvvec]:{
   .fmask = 0;
   .fval  = 0;
   forall mvvec [f]: {
      fmask = bitor fmask f.0!;
      fval  = bitor fval f.1!;
   }!;
   <fmask, fval>
}

set rdfield[_help="<field> <val> - extract field from value",field,regval]:{
   if regval != NULL {
      .mask = fieldmask field.0 field.1!;
      shiftr (bitand regval mask!) field.0!
   }{NULL}!
}

set rdfield_from[rdfn,field,handle]:{
   .fd = NULL;
   .reg = rdfn handle!;
   reg != NULL { fd = rdfield field reg! }!;
   fd
}

set rdfield_from_eq[rdfn,field,val,handle]:{
   .ok = FALSE;
   .reg = rdfn handle!;
   reg != NULL { ok = (val == (rdfield field reg!)) }!;
   ok
}

# take list of <field,value> pairs and calculate the corresponding <mask,value>
set field_sum[fvvec]:{
   .fmask = 0;
   .fval  = 0;
   .fm    = 0;
   forall fvvec [fv]:{
      fm = fieldmask fv.0.0 fv.0.1!;
      fmask = bitor fmask fm!;
      fval  = bitor fval (bitand (shiftl fv.1 fv.0.0!) fm!)!;
   }!;
   <fmask, fval>
}

# take environment of field assignments and calculate the corresponding
# <mask,value>
set fielddef_sum[fdef, assign]:{
   .fmask = 0;
   .fval  = 0;
   .fm    = 0;
   .f     = NULL;
   forall assign [fv,fname]:{
      inenv fdef fname! {
         f = fdef.(fname);
         fm = fieldmask f.0 f.1!;
         fmask = bitor fmask fm!;
         fval  = bitor fval (bitand (shiftl fv f.0!) fm!)!;
      }!
   }!;
   <fmask, fval>
}

set fielddef_val[fdef, assign]:{(fielddef_sum fdef assign!).1}

# encode a number as an environment with the same field names as a field defn.
set fielddef_get[fdef,fval]:{
   .fenv = [];
   forall fdef [field,name]:{fenv.(name) = rdfield field fval!;}!;
   fenv
}

set reg_update[rdregfn,wrregfn,mask,handle,value]:{
   .ret = FALSE;
   .regval = rdregfn handle!;
   regval != NULL {
      .oldb = bitand regval (bitnot mask!)!;
      .newb = bitand value mask!;
      ret = wrregfn (bitor oldb newb!) handle!;
   }!;
   ret
}

# deprecated
set reg_updatefields[rdregfn,wrregfn,fvvec,handle]:{
   .fv = field_sum fvvec!; # mask, value
   reg_update rdregfn wrregfn fv.0 handle fv.1!
}

set reg_update_fields[rdregfn,wrregfn,fdef,fenv,handle]:{
   .fv = fielddef_sum fdef fenv!; # mask, value
   reg_update rdregfn wrregfn fv.0 handle fv.1!
}


# display value decoded as a directory of fields
set showfields[_help="<fields> <val> - display fields in the value",
               fields,val]:{
   .fval = NULL;
   forall fields [field,name]:{
      (typeof name!) == type_int { name = strf "%d" <name>!; }!;
      fval = rdfield field val!;
      if (fval==NULL) {
         printf "%16s: %-7s <not available>\n" <
                name, strf "[%d,%d]" <field.0, field.1>!>!;
      }{
         printf "%16s: %-7s %-5d %s\n" <
                name, strf "[%d,%d]" <field.0, field.1>!, fval,
                if (inenv field 2!) {
                   if ((typeof field.2!) == type_str) {field.2} {field.2 fval!}!
                }{""}!>!;

      }!
   }!
}

set showbitset[outffn, delim, pfx, field, val]:{
   .shown=FALSE;
   forall <field.0 .. field.0+field.1-1> [bit]:{
      0 != (bitand val (bitmask bit!)!) {
         if shown {outffn "%s" <delim>!} {shown=TRUE}!;
         outffn "%s%d" <pfx, bit>!;
      }!;
   }!;
}

set strbitset[delim, pfx, field, val]:{
   io.outstring [stream]:{showbitset (io.fprintf stream) delim pfx field val!}!
}

# useful functions for the third component of a field definition (display)
set boolmsg[if=if,clearmsg,setmsg,n]::{if n==0 {clearmsg}{setmsg}!}
set boolmsgf[strf=strf,if=if,format, no, yes, n]::{strf format <if n==0 {no}{yes}!>!}
set intmsg[strf=strf,format,n]::{strf format <n>!}
set choicemsg[choices,n]::{choices.(n)}
set choiceormsg[if=if,inenv=inenv,choices,elsefn,n]::{
    if (inenv choices n!) {choices.(n)} {elsefn n!}!
}


