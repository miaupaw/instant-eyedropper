####################################################################################################
# Build Instant Eyedropper

require 'FileUtils'
require 'colored'

$debug = false

####################################################################################################

$currentdir = File.expand_path(File.dirname(__FILE__))
$tmp        = ENV["tmp"]
$progs      = ENV["progs"]
$vs         = ENV["vs"]

$version    = false
$eyeproject = "#{$currentdir}"

$devenv     = "#{$vs}/Common7/IDE/devenv.com";
$solution   = "#{$eyeproject}/vsproject/instanteyedropper.sln";

$inno       = "#{$progs}/Inno Setup 5/ISCC.exe";
$innoscript = "#{$eyeproject}/makeinstaller.iss";

$sevenz     = "#{$progs}/7Zip/7z.exe";
$zipname    = "instant-eyedropper"
$newdir     = "#{$tmp}/#{$zipname}"

# добавляет кавычки к строке #######################################################################
class String; def addq; "\"#{self}\""; end; end

####################################################################################################
# запуск внешней команды с сохранением вывода ######################################################
# если в выводе попадется true_pattern, то возращаем его индекс
def sysrun(command, true_patterns, debug)

    # запустить и считать вывод
    #show_wait_spinner ['green!', 30] {
        $lines = IO.popen(command + " 2>&1").readlines.join.split("\n") #}
        
    # если включен режим дебага, то вывести все на экран
    if debug
        puts # декоративный отступ 
        puts "\t" + "command: ".bold.black + command.to_s.green
        puts "\t" + "true patterns: ".bold.black + true_patterns.to_s.yellow
        $lines.each { |line| puts "\t" + line.to_s.cyan }
    end

    # в случае если есть пустой паттерн соответствия, но также пустой вывод - возвращаем тру
    true_patterns.each { |pattern| return 0 if (pattern.empty? && $lines.empty?) }
    true_patterns.delete("") # удаляем пустые паттерны, чтобы следующая проверка не заглючила

    # пройтись по строкам и паттернам
    $lines.each do |line| 
        true_patterns.each_with_index { |pattern, indx| return indx if (line.match pattern) }
    end

    return false
end

# добавляет кавычки к строке
class String; def addq; "\"#{self}\""; end; end

####################################################################################################

    # verbose
    if ARGV[0].eql?("debug") then $debug = true end

    # узнать текущую версию
    version_regexp = Regexp.new '(\d+\.\d+\.\d+\.\d+)'
    vlines = IO.popen("ruby version.rb").readlines.join.split("\n")
    vlines.each do |line| if( m = line.match version_regexp ) then $version = m[1]; break end end
    
    if $version

        $version.gsub!( /\.\d+$/, '')

        # скомпилить проект
        @command = "#{$devenv} #{$solution.addq} /Rebuild Release"

        if sysrun(@command, ["Rebuild All: 1 succeeded"], $debug)
            if $debug then puts "\t" + "ok".bold.green
            else           puts "\t" + "compile project: " + "ok".bold.green end
        else
            puts "\t" + "compilation no ok".bold.red; exit
        end

        # собрать инсталлятор
        @command = "#{$inno.addq} #{$innoscript.addq}"
        if sysrun(@command, ["Successful compile"], $debug)
            if $debug then puts "\t" + "ok".bold.green
            else           puts "\t" + "build installer: " + "ok".bold.green end
        else
            puts "\t" + "building installer failed".bold.red; exit
        end

        # собрать zip
        $src = "#{$currentdir}/vsproject/output"
        $zip = $zipname + '-' + $version + '.zip'
        FileUtils.rm($zip) if File.exist?($zip)
        FileUtils.cp_r $src, $newdir

        @command = "#{$sevenz.addq} -tzip a #{$zip} #{$newdir}";
        if sysrun(@command, ["Everything is Ok"], $debug)
            if $debug then puts "\t" + "ok".bold.green
            else           puts "\t" + "make zip: " + "ok".bold.green end
        else
            puts "\t" + "zip won't create".bold.red; exit
        end

        FileUtils.rm_rf($newdir) if File.exist?($newdir)
    end   
