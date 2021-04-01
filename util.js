
function convertToDurationString(n) {
    n /= 1000
    var zeroPad = function(num) {
        return (num < 10 ? "0" : "") + num
    }
    var minutes = Math.floor(n / 60)
    var seconds = Math.ceil(n % 60)
    return minutes + ":" + zeroPad(seconds, 2)
}
