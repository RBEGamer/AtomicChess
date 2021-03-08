function randomInteger(_min, _max) {
    return Math.floor(Math.random() * (_max - _min + 1)) + _min;
}


module.exports = {
    randomInteger
}