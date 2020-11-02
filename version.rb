####################################################################################################
# Version 1.0

require 'FileUtils'
require 'colored'

$debug = true

####################################################################################################

$currentdir     = File.expand_path(File.dirname(__FILE__))
$home           = ENV["home"]
$tmp            = ENV["tmp"]

$rcfile         = "resource.h"      # файл заголовка ресурсов где хранится номер версии
$vmajor         = "version_major"   # ключевое слово в .h файле
$vminor         = "version_minor"   # ключевое слово в .h файле
$vbuild         = "version_build"   # ключевое слово в .h файле
$vrevis         = "version_revis"   # ключевое слово в .h файле

$major          = false
$minor          = false
$build          = false
$revis          = false

####################################################################################################
# main
    
    # шаблоны для поиска
    regex_major = Regexp.new "#define #{Regexp.escape($vmajor)}.*(\\d+)$"
    regex_minor = Regexp.new "#define #{Regexp.escape($vminor)}.*(\\d+)$"
    regex_build = Regexp.new "#define #{Regexp.escape($vbuild)}.*(\\d+)$"
    regex_revis = Regexp.new "#define #{Regexp.escape($vrevis)}.*(\\d+)$"

    # выцепим для начала текущую версию проекта
    require 'find'

    files = []
    Find.find($currentdir) do |path| files << path if path.downcase.include?($rcfile) end

    if $debug
        if    files.size == 1 then puts "\t" + "found .h file".to_s.green
        elsif files.size >= 2 then puts "\t" + "too many header files found".bold.red; exit
        else                       puts "\t" + "header file not found".bold.red; exit; end
    end

    File.open(files[0], "r:UTF-8").each do |line|
        if( m = line.match regex_major ) then $major = m[1].to_i end
        if( m = line.match regex_minor ) then $minor = m[1].to_i end
        if( m = line.match regex_build ) then $build = m[1].to_i end
        if( m = line.match regex_revis ) then $revis = m[1].to_i end
    end
    
    if [$major, $minor, $build, $revis].all? { |i| i.is_a?(Integer) } # все ли цифры?

        versionstr = "#{$major}.#{$minor}.#{$build}.#{$revis}"

        puts "\t" + "current version: " + versionstr.yellow if $debug
        puts versionstr unless $debug
    end

    # пропишем новую версию
    if ARGV[0] then $newversion = ARGV[0] else $newversion = ""; exit; end

    if $newversion.match '^\d+\.\d+\.\d+\.\d+$'

        tmp_major = $newversion.split('.')[0]
        tmp_minor = $newversion.split('.')[1]
        tmp_build = $newversion.split('.')[2]
        tmp_revis = $newversion.split('.')[3]

        versionstring = "#{tmp_major}.#{tmp_minor}.#{tmp_build}.#{tmp_revis}"
        puts "\t" + "new version:     " + versionstring.magenta if $debug
        
        file_contents = File.read(files[0]).force_encoding("UTF-8")
        
        file_contents.gsub! regex_major, "#define #{$vmajor}   #{tmp_major}"
        file_contents.gsub! regex_minor, "#define #{$vminor}   #{tmp_minor}"
        file_contents.gsub! regex_build, "#define #{$vbuild}   #{tmp_build}"
        file_contents.gsub! regex_revis, "#define #{$vrevis}   #{tmp_revis}"

        File.open(files[0], "w:UTF-8") {|file| file.puts file_contents }
    end
    