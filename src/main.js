import * as p5 from 'p5'
import moment from 'moment'

function getRndInteger (min, max) {
  return Math.floor(Math.random() * (max - min)) + min
}

const sketch = (sk) => {
  const STEP = 4
  const GEN_SIZE = 5
  const MIN_GEN = 3
  const MAX_GEN = 10
  const ELLIPSE_COLOR_START_G = 200
  const ELLIPSE_COLOR_START_B = 150

  let x = 0
  let y

  sk.ellipseR = 20
  sk.ellipseG = ELLIPSE_COLOR_START_G
  sk.ellipseB = ELLIPSE_COLOR_START_B
  sk.ellipseCollor = sk.color(sk.ellipseR, sk.ellipseG, sk.ellipseB)
  sk.rectColor = sk.color(5, 0, 8)
  sk.rectColor.setAlpha(40)

  sk.logGen = (genSize) => {
    return function (x) {
      return sk.log(x) ** genSize
    }
  }

  sk.drawTime = () => {
    const x = sk.width / 2 - 100
    const y = sk.height / 2 + 10

    sk.strokeWeight(1)
    sk.textSize(40)

    sk.stroke(150, 250, 250)

    sk.textFont(sk.font)
    sk.text(moment().format('H : mm : ss '), x, y)

    sk.textSize(18)
    sk.textFont(sk.dateFont)

    sk.text(moment().format('M / DD '), x, y + 30)
  }

  sk.drawEllipse = (x, y) => {
    sk.noFill()
    sk.ellipseCollor = sk.color(sk.ellipseR, sk.ellipseG, sk.ellipseB)
    sk.strokeWeight(2)
    sk.stroke(sk.ellipseCollor)
    sk.ellipse(sk.width / 2, sk.height / 2, y, x)
  }

  sk.drawRect = () => {
    sk.fill(sk.rectColor)
    sk.noStroke()
    sk.rect(0, 0, sk.width, sk.height)
  }

  sk.setup = () => {
    sk.createCanvas(sk.windowWidth, sk.windowHeight)
    sk.background(5, 0, 8)

    sk.equation = sk.logGen(GEN_SIZE)

    sk.font = sk.loadFont('assets/fonts/Roboto-Black.ttf')
    sk.dateFont = sk.loadFont('assets/fonts/Roboto-Light.ttf')
  }

  sk.draw = () => {
    sk.drawRect()
    x += STEP
    y = sk.equation(x)

    sk.drawEllipse(x, y)
    sk.drawTime()

    if (x > sk.width) {
      x = 0
      const genSize = getRndInteger(MAX_GEN, MIN_GEN)
      sk.equation = sk.logGen(genSize)
      console.log('Wave gen size:', genSize)
    }
  }
}

const P5 = new p5(sketch)
