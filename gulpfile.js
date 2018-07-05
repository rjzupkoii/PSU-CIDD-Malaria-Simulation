var gulp = require("gulp");
var exec = require("child_process").exec;
var spawn = require("child_process").spawnSync;

gulp.task("wt", function () {
    gulp.watch("bin/masim_test.exe", ["test"]);
});

gulp.task("wd", function () {
    gulp.watch("bin/MaSim.exe", ["dev"]);
});

gulp.task("test", function () {
	spawn("bin/masim_test.exe", {
        stdio: "inherit" // <== IMPORTANT: use this option to inherit the parent's environment
    });
});
gulp.task("dev", function () {
  spawn("bin/MaSim.exe",["-i","misc\\input.yml"], {
        stdio: "inherit" // <== IMPORTANT: use this option to inherit the parent's environment
    });
});


//
//var cmakeCommand = "mkdir -p build; cd build; cmake ..;";
//var cleanCommand = "rm -rf build";
//var testCommand = "cd build; ctest -V";
//
////"cmake --build ."
//var buildCommand = "cd build; make -j 2;";
//var runCommand = "cd build; make run;"
//
//gulp.task('clean', function () {
//    exec(cleanCommand, function (error, stdout, stderr) {
//        console.log(stdout);
//        console.log(stderr);
//    });
//});
//
//
//gulp.task('cmake', function (cb) {
//    exec(cmakeCommand, function (error, stdout, stderr) {
//        console.log(stdout);
//        console.log(stderr);
//        cb(stderr);
//    });
//
//});
//
//gulp.task('build', function (cb) {
//    exec(buildCommand, function (error, stdout, stderr) {
//        console.log(stdout);
//        console.log(stderr);
//        cb(stderr);
//    });
//});
//
//
//gulp.task('test', ['build'], function () {
//    exec(testCommand, function (error, stdout, stderr) {
//        console.log(stdout);
//        console.log(stderr);
//    });
//});
//
//
//gulp.task('run', ['build'], function () {
//    exec(runCommand, function (error, stdout, stderr) {
//        console.log(stdout);
//        console.log(stderr);
//    });
//});
//
//gulp.task('wt', function () {
//    gulp.watch('src/**/*.cpp', ['test']);
//    gulp.watch('src/**/*.h', ['test']);
//    gulp.watch('include/**/*.h', ['test']);
//    gulp.watch('test/**/*.cpp', ['test']);
//    gulp.watch('test/**/*.h', ['test']);
//});
//
//gulp.task('wr', function () {
//    gulp.watch('src/**/*.cpp', ['run']);
//    gulp.watch('src/**/*.h', ['run']);
//    gulp.watch('include/**/*.h', ['run']);
//});
//
//gulp.task('default', ['build', 'test'], function () { });
//gulp.task('c', ['cmake'], function () { });
//gulp.task('t', ['test'], function () { });
//gulp.task('b', ['build'], function () { });
//gulp.task('r', ['run'], function () { });