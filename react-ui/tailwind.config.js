/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        sunset: {
          50: '#fff8f1',
          100: '#ffeedc',
          200: '#ffd8b1',
          300: '#ffbe80',
          400: '#ff9f4d',
          500: '#f67f1f',
          600: '#dc6510',
          700: '#b84f0f',
          800: '#934110',
          900: '#783511',
        },
        ember: {
          50: '#fff3ed',
          100: '#ffe1d1',
          200: '#ffc09d',
          300: '#ff9b68',
          400: '#ff7440',
          500: '#ff4f1f',
          600: '#f53c12',
          700: '#c92d10',
          800: '#a32813',
          900: '#842316',
        },
      },
      backdropBlur: {
        xs: '2px',
      },
    },
  },
  plugins: [],
}
