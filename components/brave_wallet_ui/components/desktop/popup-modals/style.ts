import styled from 'styled-components'
import CloseIcon from '../../extension/assets/close.svg'

interface StyleProps {
  isFlexible: boolean
}

export const StyledWrapper = styled.div`
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  left: 0;
  right: 0;
  top: 0;
  bottom: 0;
  position: fixed;
  z-index: 10;
  background: rgba(33, 37, 41, 0.32);
  backdrop-filter: blur(16px);
`

export const Modal = styled.div<StyleProps>`
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-conent: center;
  min-width: ${(p) => p.isFlexible ? 'none' : '580px'};
  max-width: ${(p) => p.isFlexible ? 'none' : '580px'};
  background-color: ${(p) => p.theme.color.background02};
  border-radius: 8px;
  box-shadow: 0px 0px 15px rgba(0, 0, 0, 0.25);
  @media (prefers-color-scheme: dark) {
    border: 1px solid ${(p) => p.theme.color.divider01};
    box-shadow: 0px 0px 16px rgba(0, 0, 0, 0.56);
  }
`

export const Header = styled.div`
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-direction: row;
  padding: 20px;
  width: 100%;
`

export const Title = styled.span`
  font-family: Poppins;
  font-size: 18px;
  font-weight: 600;
  letter-spacing: 0.02em;
  line-height: 26px;
  color: ${(p) => p.theme.color.text01};
`

export const CloseButton = styled.button`
  display: flex;;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  width: 20px;
  height: 20px;
  background-color: ${(p) => p.theme.color.interactive07};
  -webkit-mask-image: url(${CloseIcon});
  mask-image: url(${CloseIcon});
  outline: none;
  border: none;
`
