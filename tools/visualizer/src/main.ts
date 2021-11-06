export function foo () {
  const element = document.createElement('div')

  element.innerHTML = 'Hello, world'

  document.body.appendChild(element)
}
