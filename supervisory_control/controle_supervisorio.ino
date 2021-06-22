//retorna 1 se o evento estiver desabilitado por pelo menos um supervisor e 
//retorna 0 caso contrário
bool verifica_evento(int evento)
{
  return S1.verifica(evento); //|| S2.verifica(evento);
}
