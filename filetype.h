#ifndef MODLS_FILETYPE_H
#define MODLS_FILETYPE_H
#include <string>
#include <map>
#include <cstring>

inline std::string get_extension(const std::string& filename){
  const char* sfilename=filename.c_str();
  int i=filename.find_last_of("./");
  if(i>=0&&sfilename[i]=='.'){
    const char* ext=sfilename+i;
    int len=filename.length()-i;
    if(len>4&&std::strcmp("\33[0m",ext+len-4)==0)len-=4;
    return std::string(ext,len);
  }else{
    return "";
  }
}

struct ft{
  static const int image   =1;
  static const int archive =2;
  static const int media   =3;

  typedef std::map<std::string,int> map_t;
  static void init_map(map_t& map){
    // raster images
    map[".bmp"]=ft::image;
    map[".cd5"]=ft::image;
    map[".ecw"]=ft::image;
    map[".fits"]=ft::image;
    map[".gif"]=ft::image;
    map[".ico"]=ft::image;
    map[".ilbm"]=ft::image;
    map[".img"]=ft::image;
    map[".jfif"]=ft::image;
    map[".jpeg"]=ft::image;
    map[".jpg"]=ft::image;
    map[".mng"]=ft::image;
    map[".pbm"]=ft::image;
    map[".pcx"]=ft::image;
    map[".pgf"]=ft::image;
    map[".pgm"]=ft::image;
    map[".png"]=ft::image;
    map[".pnm"]=ft::image;
    map[".ppm"]=ft::image;
    map[".raw"]=ft::image;
    map[".rle"]=ft::image;
    map[".sid"]=ft::image;
    map[".tga"]=ft::image;
    map[".tif"]=ft::image;
    map[".tiff"]=ft::image;
    map[".xbm"]=ft::image;
    map[".xcf"]=ft::image;
    map[".xpm"]=ft::image;
    map[".xwd"]=ft::image;

    // vector images
    map[".cdr"]=ft::image;
    map[".cgm"]=ft::image;
    map[".emf"]=ft::image;
    map[".eps"]=ft::image;
    map[".jps"]=ft::image;
    map[".mpo"]=ft::image;
    map[".odg"]=ft::image;
    map[".pdf"]=ft::image;
    map[".pgml"]=ft::image;
    map[".pns"]=ft::image;
    map[".ps"]=ft::image;
    map[".svg"]=ft::image;
    map[".svgz"]=ft::image;
    map[".swf"]=ft::image;
    map[".vml"]=ft::image;
    map[".wmf"]=ft::image;
    map[".wml"]=ft::image;
    map[".xps"]=ft::image;

    // archives
    map[".7z"]=ft::archive;
    map[".Z"]=ft::archive;
    map[".ace"]=ft::archive;
    map[".arc"]=ft::archive;
    map[".arj"]=ft::archive;
    map[".b64"]=ft::archive;
    map[".bh"]=ft::archive;
    map[".bz"]=ft::archive;
    map[".bz2"]=ft::archive;
    map[".bza"]=ft::archive;
    map[".cab"]=ft::archive;
    map[".cpio"]=ft::archive;
    map[".cpt"]=ft::archive;
    map[".deb"]=ft::archive;
    map[".dz"]=ft::archive;
    map[".gca"]=ft::archive;
    map[".gz"]=ft::archive;
    map[".gza"]=ft::archive;
    map[".hqx"]=ft::archive;
    map[".imp"]=ft::archive;
    map[".ish"]=ft::archive;
    map[".jar"]=ft::archive;
    map[".lz"]=ft::archive;
    map[".lza"]=ft::archive;
    map[".lzh"]=ft::archive;
    map[".lzma"]=ft::archive;
    map[".lzs"]=ft::archive;
    map[".mim"]=ft::archive;
    map[".mime"]=ft::archive;
    map[".pak"]=ft::archive;
    map[".rar"]=ft::archive;
    map[".rpm"]=ft::archive;
    map[".rz"]=ft::archive;
    map[".sea"]=ft::archive;
    map[".sit"]=ft::archive;
    map[".tar"]=ft::archive;
    map[".taz"]=ft::archive;
    map[".tbz"]=ft::archive;
    map[".tbz2"]=ft::archive;
    map[".tgz"]=ft::archive;
    map[".tlz"]=ft::archive;
    map[".txz"]=ft::archive;
    map[".tz"]=ft::archive;
    map[".uu"]=ft::archive;
    map[".uue"]=ft::archive;
    map[".xxe"]=ft::archive;
    map[".xz"]=ft::archive;
    map[".yz1"]=ft::archive;
    map[".z"]=ft::archive;
    map[".zip"]=ft::archive;
    map[".zoo"]=ft::archive;

    // media
    map[".3g2"]=ft::media;
    map[".3gp"]=ft::media;
    map[".3gp2"]=ft::media;
    map[".3gpp"]=ft::media;
    map[".aac"]=ft::media;
    map[".ac3"]=ft::media;
    map[".amv"]=ft::media;
    map[".anx"]=ft::media;
    map[".asf"]=ft::media;
    map[".asx"]=ft::media;
    map[".au"]=ft::media;
    map[".avi"]=ft::media;
    map[".avs"]=ft::media;
    map[".axa"]=ft::media;
    map[".axv"]=ft::media;
    map[".divx"]=ft::media;
    map[".dl"]=ft::media;
    map[".dv"]=ft::media;
    map[".f4v"]=ft::media;
    map[".flac"]=ft::media;
    map[".flc"]=ft::media;
    map[".fli"]=ft::media;
    map[".flv"]=ft::media;
    map[".gl"]=ft::media;
    map[".m1v"]=ft::media;
    map[".m2p"]=ft::media;
    map[".m2t"]=ft::media;
    map[".m2ts"]=ft::media;
    map[".m2v"]=ft::media;
    map[".m4a"]=ft::media;
    map[".m4v"]=ft::media;
    map[".mid"]=ft::media;
    map[".midi"]=ft::media;
    map[".mka"]=ft::media;
    map[".mkv"]=ft::media;
    map[".mod"]=ft::media;
    map[".mov"]=ft::media;
    map[".mp2"]=ft::media;
    map[".mp3"]=ft::media;
    map[".mp4"]=ft::media;
    map[".mp4v"]=ft::media;
    map[".mpc"]=ft::media;
    map[".mpe"]=ft::media;
    map[".mpeg"]=ft::media;
    map[".mpeg1"]=ft::media;
    map[".mpeg2"]=ft::media;
    map[".mpeg4"]=ft::media;
    map[".mpg"]=ft::media;
    map[".mpv"]=ft::media;
    map[".mts"]=ft::media;
    map[".nsv"]=ft::media;
    map[".nuv"]=ft::media;
    map[".oga"]=ft::media;
    map[".ogg"]=ft::media;
    map[".ogm"]=ft::media;
    map[".ogv"]=ft::media;
    map[".ogx"]=ft::media;
    map[".qt"]=ft::media;
    map[".ra"]=ft::media;
    map[".ram"]=ft::media;
    map[".rm"]=ft::media;
    map[".rmvb"]=ft::media;
    map[".rv"]=ft::media;
    map[".spx"]=ft::media;
    map[".swf"]=ft::media;
    map[".tod"]=ft::media;
    map[".tp"]=ft::media;
    map[".trp"]=ft::media;
    map[".ts"]=ft::media;
    map[".vob"]=ft::media;
    map[".vro"]=ft::media;
    map[".wav"]=ft::media;
    map[".wma"]=ft::media;
    map[".wmv"]=ft::media;
    map[".xspf"]=ft::media;
    map[".yuv"]=ft::media;

    //map[""]=ft::media;
  }
};

#endif
